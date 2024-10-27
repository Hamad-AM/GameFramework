#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include <donut/core/vfs/VFS.h>
#include <donut/core/log.h>
#include <donut/core/string_utils.h>
#include <donut/engine/CommonRenderPasses.h>
#include <donut/engine/ConsoleInterpreter.h>
#include <donut/engine/ConsoleObjects.h>
#include <donut/engine/FramebufferFactory.h>
#include <donut/engine/Scene.h>
#include <donut/engine/ShaderFactory.h>
#include <donut/engine/TextureCache.h>
#include <donut/render/DepthPass.h>
#include <donut/render/DrawStrategy.h>
#include <donut/render/ForwardShadingPass.h>
#include <donut/render/GBuffer.h>
#include <donut/render/GBufferFillPass.h>
#include <donut/render/PixelReadbackPass.h>
#include <donut/render/SkyPass.h>
#include <donut/render/MipMapGenPass.h>
#include <donut/app/ApplicationBase.h>
#include <donut/app/UserInterfaceUtils.h>
#include <donut/app/Camera.h>
#include <donut/app/DeviceManager.h>
#include <donut/app/imgui_console.h>
#include <donut/app/imgui_renderer.h>
#include <nvrhi/utils.h>
#include <nvrhi/common/misc.h>

using namespace donut;
using namespace donut::math;
using namespace donut::app;
using namespace donut::vfs;
using namespace donut::engine;
using namespace donut::render;

class RenderTargets : public GBufferRenderTargets
{
public:
    nvrhi::TextureHandle HdrColor;
    nvrhi::TextureHandle LdrColor;
    nvrhi::TextureHandle MaterialIDs;
    nvrhi::TextureHandle ResolvedColor;
    nvrhi::TextureHandle TemporalFeedback1;
    nvrhi::TextureHandle TemporalFeedback2;
    nvrhi::TextureHandle AmbientOcclusion;

    nvrhi::HeapHandle Heap;

    std::shared_ptr<FramebufferFactory> ForwardFramebuffer;
    std::shared_ptr<FramebufferFactory> HdrFramebuffer;
    std::shared_ptr<FramebufferFactory> LdrFramebuffer;
    std::shared_ptr<FramebufferFactory> ResolvedFramebuffer;
    std::shared_ptr<FramebufferFactory> MaterialIDFramebuffer;
    
    void Init(
        nvrhi::IDevice* device,
        dm::uint2 size,
        dm::uint sampleCount,
        bool enableMotionVectors,
        bool useReverseProjection) override
    {
        GBufferRenderTargets::Init(device, size, sampleCount, enableMotionVectors, useReverseProjection);
        
        nvrhi::TextureDesc desc;
        desc.width = size.x;
        desc.height = size.y;
        desc.isRenderTarget = true;
        desc.useClearValue = true;
        desc.clearValue = nvrhi::Color(1.f);
        desc.sampleCount = sampleCount;
        desc.dimension = sampleCount > 1 ? nvrhi::TextureDimension::Texture2DMS : nvrhi::TextureDimension::Texture2D;
        desc.keepInitialState = true;
        desc.isVirtual = device->queryFeatureSupport(nvrhi::Feature::VirtualResources);

        desc.clearValue = nvrhi::Color(0.f);
        desc.isTypeless = false;
        desc.isUAV = sampleCount == 1;
        desc.format = nvrhi::Format::RGBA16_FLOAT;
        desc.initialState = nvrhi::ResourceStates::RenderTarget;
        desc.debugName = "HdrColor";
        HdrColor = device->createTexture(desc);

        desc.format = nvrhi::Format::RG16_UINT;
        desc.isUAV = false;
        desc.debugName = "MaterialIDs";
        MaterialIDs = device->createTexture(desc);

        // The render targets below this point are non-MSAA
        desc.sampleCount = 1;
        desc.dimension = nvrhi::TextureDimension::Texture2D;

        desc.format = nvrhi::Format::RGBA16_FLOAT;
        desc.isUAV = true;
        desc.mipLevels = uint32_t(floorf(::log2f(float(std::max(desc.width, desc.height)))) + 1.f); // Used to test the MipMapGen pass
        desc.debugName = "ResolvedColor";
        ResolvedColor = device->createTexture(desc);

        desc.format = nvrhi::Format::RGBA16_SNORM;
        desc.mipLevels = 1;
        desc.debugName = "TemporalFeedback1";
        TemporalFeedback1 = device->createTexture(desc);
        desc.debugName = "TemporalFeedback2";
        TemporalFeedback2 = device->createTexture(desc);

        desc.format = nvrhi::Format::SRGBA8_UNORM;
        desc.isUAV = false;
        desc.debugName = "LdrColor";
        LdrColor = device->createTexture(desc);

        desc.format = nvrhi::Format::R8_UNORM;
        desc.isUAV = true;
        desc.debugName = "AmbientOcclusion";
        AmbientOcclusion = device->createTexture(desc);

        if (desc.isVirtual)
        {
            uint64_t heapSize = 0;
            nvrhi::ITexture* const textures[] = {
                HdrColor,
                MaterialIDs,
                ResolvedColor,
                TemporalFeedback1,
                TemporalFeedback2,
                LdrColor,
                AmbientOcclusion
            };

            for (auto texture : textures)
            {
                nvrhi::MemoryRequirements memReq = device->getTextureMemoryRequirements(texture);
                heapSize = nvrhi::align(heapSize, memReq.alignment);
                heapSize += memReq.size;
            }

            nvrhi::HeapDesc heapDesc;
            heapDesc.type = nvrhi::HeapType::DeviceLocal;
            heapDesc.capacity = heapSize;
            heapDesc.debugName = "RenderTargetHeap";

            Heap = device->createHeap(heapDesc);

            uint64_t offset = 0;
            for (auto texture : textures)
            {
                nvrhi::MemoryRequirements memReq = device->getTextureMemoryRequirements(texture);
                offset = nvrhi::align(offset, memReq.alignment);

                device->bindTextureMemory(texture, Heap, offset);

                offset += memReq.size;
            }
        }
        
        ForwardFramebuffer = std::make_shared<FramebufferFactory>(device);
        ForwardFramebuffer->RenderTargets = { HdrColor };
        ForwardFramebuffer->DepthTarget = Depth;

        HdrFramebuffer = std::make_shared<FramebufferFactory>(device);
        HdrFramebuffer->RenderTargets = { HdrColor };

        LdrFramebuffer = std::make_shared<FramebufferFactory>(device);
        LdrFramebuffer->RenderTargets = { LdrColor };

        ResolvedFramebuffer = std::make_shared<FramebufferFactory>(device);
        ResolvedFramebuffer->RenderTargets = { ResolvedColor };

        MaterialIDFramebuffer = std::make_shared<FramebufferFactory>(device);
        MaterialIDFramebuffer->RenderTargets = { MaterialIDs };
        MaterialIDFramebuffer->DepthTarget = Depth;
    }

    [[nodiscard]] bool IsUpdateRequired(uint2 size, uint sampleCount) const
    {
        if (any(m_Size != size) || m_SampleCount != sampleCount)
            return true;

        return false;
    }

    void Clear(nvrhi::ICommandList* commandList) override
    {
        GBufferRenderTargets::Clear(commandList);

        commandList->clearTextureFloat(HdrColor, nvrhi::AllSubresources, nvrhi::Color(0.f));
    }
};

class Game : public ApplicationBase
{
private:
    typedef ApplicationBase Super;
    std::shared_ptr<RootFileSystem>     m_RootFs;
	std::shared_ptr<Scene>				m_Scene;
	std::shared_ptr<ShaderFactory>      m_ShaderFactory;
    std::shared_ptr<DirectionalLight>   m_SunLight;
    std::shared_ptr<InstancedOpaqueDrawStrategy> m_OpaqueDrawStrategy;
    std::shared_ptr<TransparentDrawStrategy> m_TransparentDrawStrategy;
    std::unique_ptr<RenderTargets>      m_RenderTargets;
    std::shared_ptr<ForwardShadingPass> m_ForwardPass;
    std::unique_ptr<MaterialIDPass>     m_MaterialIDPass;
    std::unique_ptr<PixelReadbackPass>  m_PixelReadbackPass;
    std::unique_ptr<MipMapGenPass>      m_MipMapGenPass;

    std::shared_ptr<IView>              m_View;
    std::shared_ptr<IView>              m_ViewPrevious;

    nvrhi::CommandListHandle            m_CommandList;
    bool                                m_PreviousViewsValid = false;
    FirstPersonCamera                   m_FirstPersonCamera;
    ThirdPersonCamera                   m_ThirdPersonCamera;
    bool                                useThirdPerson = false;
    BindingCache                        m_BindingCache;

    float                               m_CameraVerticalFov = 60.f;
    float3                              m_AmbientTop = 0.f;
    float3                              m_AmbientBottom = 0.f;
    uint2                               m_PickPosition = 0u;

public:
    Game(DeviceManager* deviceManager)
        : Super(deviceManager)
        , m_BindingCache(deviceManager->GetDevice())
    { 
        std::shared_ptr<NativeFileSystem> nativeFS = std::make_shared<NativeFileSystem>();

        std::filesystem::path assetPath = app::GetDirectoryWithExecutable().parent_path() / "assets";
        std::filesystem::path frameworkShaderPath = app::GetDirectoryWithExecutable() / "shaders/framework" / app::GetShaderTypeName(GetDevice()->getGraphicsAPI());

        m_RootFs = std::make_shared<RootFileSystem>();
        m_RootFs->mount("/assets/", assetPath);
        m_RootFs->mount("/shaders/donut", frameworkShaderPath);
        m_RootFs->mount("/native", nativeFS);

        //TODO: find sponza file
        std::filesystem::path scenePath = "/assets/Sponza.gltf";

        m_TextureCache = std::make_shared<TextureCache>(GetDevice(), m_RootFs, nullptr);

        m_ShaderFactory = std::make_shared<ShaderFactory>(GetDevice(), m_RootFs, "/shaders");
        m_CommonPasses = std::make_shared<CommonRenderPasses>(GetDevice(), m_ShaderFactory);

        m_OpaqueDrawStrategy = std::make_shared<InstancedOpaqueDrawStrategy>();
        m_TransparentDrawStrategy = std::make_shared<TransparentDrawStrategy>();

        m_CommandList = GetDevice()->createCommandList();

        m_FirstPersonCamera.SetMoveSpeed(3.0f);
        m_ThirdPersonCamera.SetMoveSpeed(3.0f);

        SetAsynchronousLoadingEnabled(true);


        BeginLoadingScene(m_RootFs, scenePath);
    }

    std::shared_ptr<vfs::IFileSystem> GetRootFs() const
    {
        return m_RootFs;
    }

    BaseCamera& GetActiveCamera() const
    {
        return useThirdPerson ? (BaseCamera&)m_ThirdPersonCamera : (BaseCamera&)m_FirstPersonCamera;
    }

    void CopyActiveCameraToFirstPerson()
    {
        m_FirstPersonCamera.LookAt(m_ThirdPersonCamera.GetPosition(), m_ThirdPersonCamera.GetPosition() + m_ThirdPersonCamera.GetDir(), m_ThirdPersonCamera.GetUp());
    }

    virtual bool KeyboardUpdate(int key, int scancode, int action, int mods) override
    {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
            return true;	
		}

		if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
        {
			return true;
        }

        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            return true;
        }

        if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            useThirdPerson != useThirdPerson;
            return true;
        }

        GetActiveCamera().KeyboardUpdate(key, scancode, action, mods);
        return true;
    }

    virtual bool MousePosUpdate(double xpos, double ypos) override
    {
        GetActiveCamera().MousePosUpdate(xpos, ypos);
        return true;
    }

    virtual bool MouseButtonUpdate(int button, int action, int mods) override
    {
        GetActiveCamera().MouseButtonUpdate(button, action, mods);
        return true;
    }

    virtual bool MouseScrollUpdate(double xoffset, double yoffset) override
    {
        GetActiveCamera().MouseScrollUpdate(xoffset, yoffset);
        return true;
    }

    virtual void Animate(float fElapsedTimeSeconds) override
    { 
        GetActiveCamera().Animate(fElapsedTimeSeconds);
    }

    virtual void SceneUnloading() override
    {
        if (m_ForwardPass) m_ForwardPass->ResetBindingCache();
        m_BindingCache.Clear();
        m_SunLight.reset();
    }

    virtual bool LoadScene(std::shared_ptr<IFileSystem> fs, const std::filesystem::path& fileName) override
    {
        using namespace std::chrono;

        std::unique_ptr<engine::Scene> scene = std::make_unique<engine::Scene>(GetDevice(),
            *m_ShaderFactory, fs, m_TextureCache, nullptr, nullptr);

        auto startTime = high_resolution_clock::now();

        if (scene->Load(fileName))
        {
            m_Scene = std::move(scene);

            auto endTime = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(endTime - startTime).count();
            log::info("Scene loading time: %llu ms", duration);

            return true;
        }
        return false;
    }

    virtual void SceneLoaded() override
    {
        Super::SceneLoaded();

        m_Scene->FinishedLoading(GetFrameIndex());

        m_PreviousViewsValid = false;

        for (auto light : m_Scene->GetSceneGraph()->GetLights())
        {
            if (light->GetLightType() == LightType_Directional)
            {
                m_SunLight = std::static_pointer_cast<DirectionalLight>(light);
                break;
            }
        }

        if (!m_SunLight)
        {
            m_SunLight = std::make_shared<DirectionalLight>();
            m_SunLight->angularSize = 0.53f;
            m_SunLight->irradiance = 1.f;

            auto node = std::make_shared<SceneGraphNode>();
            node->SetLeaf(m_SunLight);
            m_SunLight->SetDirection(dm::double3(0.1, -0.9, 0.1));
            m_SunLight->SetName("Sun");
            m_Scene->GetSceneGraph()->Attach(m_Scene->GetSceneGraph()->GetRootNode(), node);
        }

        auto cameras = m_Scene->GetSceneGraph()->GetCameras();
        if (!cameras.empty())
        {
        }
        else
        {
            m_FirstPersonCamera.LookAt(
                float3(0.f, 1.8f, 0.f),
                float3(1.f, 1.8f, 0.f));
            m_CameraVerticalFov = 60.f;
        }

        m_ThirdPersonCamera.SetRotation(dm::radians(135.f), dm::radians(20.f));
        PointThirdPersonCameraAt(m_Scene->GetSceneGraph()->GetRootNode());

        CopyActiveCameraToFirstPerson();

        //NOTE: Print Scene graph or not
        if (false)
            PrintSceneGraph(m_Scene->GetSceneGraph()->GetRootNode());
    }

    void PointThirdPersonCameraAt(const std::shared_ptr<SceneGraphNode>& node)
    {
        dm::box3 bounds = node->GetGlobalBoundingBox();
        m_ThirdPersonCamera.SetTargetPosition(bounds.center());
        float radius = length(bounds.diagonal()) * 0.5f;
        float distance = radius / sinf(dm::radians(m_CameraVerticalFov * 0.5f));
        m_ThirdPersonCamera.SetDistance(distance);
        m_ThirdPersonCamera.Animate(0.f);
    }
    std::shared_ptr<TextureCache> GetTextureCache()
    {
        return m_TextureCache;
    }

    std::shared_ptr<Scene> GetScene()
    {
        return m_Scene;
    }

    bool SetupView()
    {
        float2 renderTargetSize = float2(m_RenderTargets->GetSize());

        std::shared_ptr<PlanarView> planarView = std::dynamic_pointer_cast<PlanarView, IView>(m_View);

        dm::affine3 viewMatrix;
        float verticalFov = dm::radians(m_CameraVerticalFov);
        float zNear = 0.01f;
        viewMatrix = GetActiveCamera().GetWorldToViewMatrix();

        bool topologyChanged = false;

        if (!planarView)
        {
            m_View = planarView = std::make_shared<PlanarView>();
            m_ViewPrevious = std::make_shared<PlanarView>();
            topologyChanged = true;
        }

        float4x4 projection = perspProjD3DStyleReverse(verticalFov, renderTargetSize.x / renderTargetSize.y, zNear);

        planarView->SetViewport(nvrhi::Viewport(renderTargetSize.x, renderTargetSize.y));
        planarView->SetPixelOffset(pixelOffset);

        planarView->SetMatrices(viewMatrix, projection);
        planarView->UpdateCache();

        m_ThirdPersonCamera.SetView(*planarView);

        if (topologyChanged)
        {
            *std::static_pointer_cast<PlanarView>(m_ViewPrevious) = *std::static_pointer_cast<PlanarView>(m_View);
        }

        return topologyChanged;
    }

    void CreateRenderPasses(bool& exposureResetRequired)
    {
        uint32_t motionVectorStencilMask = 0x01;

        ForwardShadingPass::CreateParameters ForwardParams;
        ForwardParams.trackLiveness = false;
        m_ForwardPass = std::make_unique<ForwardShadingPass>(GetDevice(), m_CommonPasses);
        m_ForwardPass->Init(*m_ShaderFactory, ForwardParams);

        GBufferFillPass::CreateParameters GBufferParams;
        GBufferParams.enableMotionVectors = true;
        GBufferParams.stencilWriteMask = motionVectorStencilMask;
        m_GBufferPass = std::make_unique<GBufferFillPass>(GetDevice(), m_CommonPasses);
        m_GBufferPass->Init(*m_ShaderFactory, GBufferParams);

        GBufferParams.enableMotionVectors = false;
        m_MaterialIDPass = std::make_unique<MaterialIDPass>(GetDevice(), m_CommonPasses);
        m_MaterialIDPass->Init(*m_ShaderFactory, GBufferParams);

        m_PixelReadbackPass = std::make_unique<PixelReadbackPass>(GetDevice(), m_ShaderFactory, m_RenderTargets->MaterialIDs, nvrhi::Format::RGBA32_UINT);
        m_MipMapGenPass = std::make_unique <MipMapGenPass>(GetDevice(), m_ShaderFactory, m_RenderTargets->ResolvedColor, MipMapGenPass::Mode::MODE_COLOR);


        m_SkyPass = std::make_unique<SkyPass>(GetDevice(), m_ShaderFactory, m_CommonPasses, m_RenderTargets->ForwardFramebuffer, *m_View);

        m_PreviousViewsValid = false;
    }

    virtual void RenderSplashScreen(nvrhi::IFramebuffer* framebuffer) override
    {
        nvrhi::ITexture* framebufferTexture = framebuffer->getDesc().colorAttachments[0].texture;
        m_CommandList->open();
        m_CommandList->clearTextureFloat(framebufferTexture, nvrhi::AllSubresources, nvrhi::Color(0.f));
        m_CommandList->close();
        GetDevice()->executeCommandList(m_CommandList);
        GetDeviceManager()->SetVsyncEnabled(true);
    }

    virtual void RenderScene(nvrhi::IFramebuffer* framebuffer) override
    {
        int windowWidth, windowHeight;
        GetDeviceManager()->GetWindowDimensions(windowWidth, windowHeight);
        nvrhi::Viewport windowViewport = nvrhi::Viewport(float(windowWidth), float(windowHeight));
        nvrhi::Viewport renderViewport = windowViewport;

        m_Scene->RefreshSceneGraph(GetFrameIndex());

        bool exposureResetRequired = false;
        {
            uint width = windowWidth;
            uint height = windowHeight;

            bool needNewPasses = false;

            if (!m_RenderTargets || m_RenderTargets->IsUpdateRequired(uint2(width, height), sampleCount))
            {
                m_RenderTargets = nullptr;
                m_BindingCache.Clear();
                m_RenderTargets = std::make_unique<RenderTargets>();
                m_RenderTargets->Init(GetDevice(), uint2(width, height), sampleCount, true, true);
                needNewPasses = true;
            }

            if (SetupView())
            {
                needNewPasses = true;
            }

            if(needNewPasses)
            {
                CreateRenderPasses(exposureResetRequired);
            }
        }

        m_CommandList->open();

        m_Scene->RefreshBuffers(m_CommandList, GetFrameIndex());

        nvrhi::ITexture* framebufferTexture = framebuffer->getDesc().colorAttachments[0].texture;
        m_CommandList->clearTextureFloat(framebufferTexture, nvrhi::AllSubresources, nvrhi::Color(0.f));

        // m_AmbientTop = m_ui.AmbientIntensity * m_ui.SkyParams.skyColor * m_ui.SkyParams.brightness;
        // m_AmbientBottom = m_ui.AmbientIntensity * m_ui.SkyParams.groundColor * m_ui.SkyParams.brightness;
        m_SunLight->shadowMap = nullptr;

        m_RenderTargets->Clear(m_CommandList);

        ForwardShadingPass::Context forwardContext;

        // if (!m_ui.UseDeferredShading || m_ui.EnableTranslucency)
        // {
        //     m_ForwardPass->PrepareLights(forwardContext, m_CommandList, m_Scene->GetSceneGraph()->GetLights(), m_AmbientTop, m_AmbientBottom, lightProbes);
        // }


        {
            RenderCompositeView(m_CommandList,
                m_View.get(), m_ViewPrevious.get(),
                *m_RenderTargets->ForwardFramebuffer,
                m_Scene->GetSceneGraph()->GetRootNode(),
                *m_OpaqueDrawStrategy,
                *m_ForwardPass,
                forwardContext,
                "ForwardOpaque",
                false);
        }

        m_SkyPass->Render(m_CommandList, *m_View, *m_SunLight, m_ui.SkyParams);

        //TODO: Render Forward pass with Transparency
        // {
        //     RenderCompositeView(m_CommandList,
        //         m_View.get(), m_ViewPrevious.get(),
        //         *m_RenderTargets->ForwardFramebuffer,
        //         m_Scene->GetSceneGraph()->GetRootNode(),
        //         *m_TransparentDrawStrategy,
        //         *m_ForwardPass,
        //         forwardContext,
        //         "ForwardTransparent",
        //         m_ui.EnableMaterialEvents);
        // }

        nvrhi::ITexture* finalHdrColor = m_RenderTargets->HdrColor;

        {
            std::shared_ptr<FramebufferFactory> finalHdrFramebuffer = m_RenderTargets->HdrFramebuffer;

            if (m_RenderTargets->GetSampleCount() > 1)
            {
                auto subresources = nvrhi::TextureSubresourceSet(0, 1, 0, 1);
                m_CommandList->resolveTexture(m_RenderTargets->ResolvedColor, subresources, m_RenderTargets->HdrColor, subresources);
                finalHdrColor = m_RenderTargets->ResolvedColor;
                finalHdrFramebuffer = m_RenderTargets->ResolvedFramebuffer;
            }

            m_PreviousViewsValid = false;
        }

        m_CommonPasses->BlitTexture(m_CommandList, framebuffer, m_RenderTargets->LdrColor, &m_BindingCache);

        m_CommandList->close();
        GetDevice()->executeCommandList(m_CommandList);

        std::swap(m_View, m_ViewPrevious);

        //TODO: Having settings change whether or not enable vsync
        // Enable VSync
        GetDeviceManager()->SetVsyncEnabled(false);
    }
    std::shared_ptr<ShaderFactory> GetShaderFactory()
    {
        return m_ShaderFactory;
    }

};

int main(int __argc, const char* const* __argv)
{
    nvrhi::GraphicsAPI api = nvrhi::GraphicsAPI::D3D11;

    DeviceCreationParameters deviceParams;

    // deviceParams.adapter = VrSystem::GetRequiredAdapter();
    deviceParams.backBufferWidth = 1280;
    deviceParams.backBufferHeight = 720;
    deviceParams.swapChainSampleCount = 1;
    deviceParams.swapChainBufferCount = 3;
    deviceParams.startFullscreen = false;
    deviceParams.vsyncEnabled = true;

    DeviceManager* deviceManager = DeviceManager::Create(api);
    const char* apiString = nvrhi::utils::GraphicsAPIToString(deviceManager->GetGraphicsAPI());

    std::string windowTitle = "Game";

    if (!deviceManager->CreateWindowDeviceAndSwapChain(deviceParams, windowTitle.c_str()))
	{
        log::error("Cannot initialize a %s graphics device with the requested parameters", apiString);
		return 1;
	}

    {
        std::shared_ptr<Game> game = std::make_shared<Game>(deviceManager);
        // std::shared_ptr<UIRenderer> gui = std::make_shared<UIRenderer>(deviceManager, game);

        // gui->Init(game->GetShaderFactory());

        deviceManager->AddRenderPassToBack(game.get());
        // deviceManager->AddRenderPassToBack(gui.get());

        deviceManager->RunMessageLoop();
    }

    deviceManager->Shutdown();
    delete deviceManager;
	
	return 0;
}
