#include "common.h"

#include "application.h"
#include "input.h"
#include "event.h"
#include "memory.h"
#include "render.h"
#include "asset_system.h"
#include "glfw_window.h"

#include <filesystem>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <sys/mman.h>
#endif

application::application() {}

application::~application()
{
    delete window;
}

void application::initialize(u32 screen_width, u32 screen_height)
{
    size_t perm_size = (size_t)4 * 1024 * 1024 * 1024;
    size_t trans_size = (size_t)256 * 1024 * 1024;

#ifdef _WIN32
    permanent_storage.base = (u8*)VirtualAlloc(nullptr, perm_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    transient_storage.base = (u8*)VirtualAlloc(nullptr, trans_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#elif __linux__
    u8* permanentBase = (u8*)mmap(
        nullptr,                    // address (let OS choose)
        perm_size,                       // size
        PROT_READ | PROT_WRITE,     // protection
        MAP_PRIVATE | MAP_ANONYMOUS,// flags
        -1,                         // fd (not used)
        0                           // offset
    );

    u8* transientBase = (u8*)mmap(
        nullptr,                    // address (let OS choose)
        perm_size,                       // size
        PROT_READ | PROT_WRITE,     // protection
        MAP_PRIVATE | MAP_ANONYMOUS,// flags
        -1,                         // fd (not used)
        0                           // offset
    );
    if (permanentBase == nullptr && transientBase == nullptr) {
        assert(false && "failed to allocate memory");
    }
#endif
    permanent_storage = InitArena(permanentBase, perm_size);
    transient_storage = InitArena(transientBase, trans_size);

    window = new glfw_window();
    screen_width_ = screen_width;
    screen_height_ = screen_height;
    window->initialize(screen_width_, screen_height_, "GameWindow");
    CompileShaders(&renderData);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(static_cast<glfw_window*>(window)->window, true);
    ImGui_ImplOpenGL3_Init();

    audio.init();

    for (const auto& entry : std::filesystem::recursive_directory_iterator("assets/asset-game/")) {
        if (entry.is_regular_file()) {
            scenes.push_back(entry.path().string());
        }
    }
}

void application::run()
{
    previous_time = window->get_time();

    GameState* state = PushStruct(&permanent_storage, GameState);

    vec3 sunPosition = { -4.6, 2.50, 2.0f };
    state->lights[0] = {
            .type = LightType::Directional,
            .name = "Sun",
            .position = sunPosition,
            .direction = glm::vec3(0.179, 2.446, 2.357),
            .color = { 1.0, 0.97, 0.92 },
            .luminance = 5,
            .isShadowCasting = 1,
    };
    // state->lights[1] = {
    //         .type = LightType::Point,
    //         .position = {3.6, 2.5, -2.9},
    //         .color = {1.0, 0.773, 0.561},
    //         .luminance = 50,
    //         .constant = 1.0, .linear=0.09, .quadratic=0.032,
    //         .isShadowCasting = 1,
    // };

    state->numberOfLights = 1;

    // vec3 lightProbePosition = {3.5, 2, -3};

    lightPosition = glm::vec3();


    RenderSetupParameters(&renderData, screen_width_, screen_height_);

    size_t asset_size = 1024 * 1024 * 1024 * (u64)2;
    MemoryArena assetArena = InitArena(PushSize(&permanent_storage, asset_size), asset_size);
    InitAssetSystem(assets, assetArena);

    LoadScene(assets, "assets/asset-game/scenes/bistro/Bistro.asset");
    UploadSceneToGPU(assets.scene, &renderData, &permanent_storage);

    camera = {};
    camera.position = glm::vec3(5.0f, 5.0f, 5.0f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.nearPlane = 0.1f;
    camera.farPlane = 1000.0f;
    camera.fov = 75.0f;
    camera.ratio = (f32)screen_width_ / (f32)screen_height_;
    camera.projection = glm::perspective(glm::radians(camera.fov), camera.ratio, camera.nearPlane, camera.farPlane);
    camera.orientation = glm::quat(glm::vec3(0.f, 0.f, 0.f));

    camera.front = glm::vec3(0.0, 0.0, -1.0);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.right = glm::normalize(glm::cross(camera.up, camera.front));
    camera.speed = 5.0f;
    camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

    lastMousePos = glm::vec2(0.0f);

    SetupEnvironmentCubeMap(&renderData);

    SetupLightsBuffer(&renderData, state->lights, state->numberOfLights);
    u32 shadow_width = 2048;
    u32 shadow_height = 2048;
    SetupShadowMapPass(&renderData, state->lights, state->numberOfLights, shadow_width, shadow_height, camera);
    // SetupPointShadowMapPass(&renderData, state->lights, state->numberOfLights, shadow_width, shadow_height);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f));

    // SetupLightProbe(&renderData);
    // RenderLightProbe(&renderData, lightProbePosition, batchMeshRenderData, model, state->lights, state->numberOfLights);

    SetupSSAOPass(&renderData);

    SetupGBuffers(&renderData);

    
    // SetupBRDFLUT(&renderData);

    b32 noSelectionMesh = true;
    u32 selectedMeshIdx;
    b32 noSelectionLight = true;
    u32 selectedLightIdx;

    s32 selectedSceneIdx = -1;

    while (!window->should_close())
    {
        window->handle_input();
        // state.audio.update();

        //std::cout << lightPosition.x << " " << lightPosition.y << std::endl;

        f64 current_time = window->get_time();
        f64 dt = current_time - previous_time;
        previous_time = current_time;

        if (dt < 0)
            dt = 0.0001;

        camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

        update(dt, state);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Scenes")) {
                if (ImGui::BeginCombo("##sceneCombo", assets.scene.name)) {
                    for (int i = 0; i < scenes.size(); ++i) {
                        bool isSelected = (selectedSceneIdx==i);
                        if (ImGui::Selectable(scenes[i].c_str(), isSelected)){
                            selectedSceneIdx = i;
                            // Load new scene
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        ImGui::Begin("Scene");
        for (u32 i = 0; i < renderData.meshCount; ++i) {
            bool isSelected = (selectedMeshIdx == i);
            if (ImGui::Selectable(renderData.meshes[i].name, isSelected)) {
                noSelectionMesh = false;
                selectedMeshIdx = i;
            }
        }
        ImGui::End();

        ImGui::Begin("Lights");
        for (u32 i = 0; i < state->numberOfLights; ++i) {
            bool isSelected = (selectedLightIdx == i);
            if (ImGui::Selectable(state->lights[i].name, isSelected)) {
                noSelectionLight = false;
                selectedLightIdx = i;
            }
        }
        if (!noSelectionLight) {
            ImGui::Separator();
            ImGui::Text("Properties");
            Light& light = state->lights[selectedLightIdx];
            if (light.type != Directional) {
                ImGui::DragFloat3("Position", &light.position[0]);
            }
            ImGui::DragFloat3("Rotation", &light.direction[0]);
        }
        ImGui::End();

        // RenderBRDFLUT(&renderData);

        UpdateLightsBuffer(&renderData, state->numberOfLights);

        vec3 lightDirection = glm::normalize(state->lights[0].direction * vec3(1));
        RenderShadowMapPass(&renderData, lightDirection, model, camera);
        RenderOmidirectionalShadowMap(&renderData, model, state->lights, state->numberOfLights);
        // //RenderEnvironmentMap(&renderData, camera);
        // RenderScene(&renderData, batchMeshRenderData, model, camera, lightPosition);
        RenderGBuffer(&renderData, model, camera);
        RenderSSAOPass(&renderData, camera);
        RenderDeferredScene(&renderData, model, camera);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window->swap_buffers();
    }
}

void application::update(f32 dt, GameState* state)
{
    // Recompile shaders
    if (input::is_key_pressed(key::r))
        CompileShaders(&renderData);
    if (input::is_key_pressed(key::p)) {
        if (mode == EditorMode) {
            mode = PlayMode;
            window->hideMouseCursor();
        } else {
            mode = EditorMode;
            window->showMouseCursor();
        }
    }

    if (mode == PlayMode) {
        vec3 changeLightPosition = vec3(0.0f);
        if (input::is_key_down(key::d))
            camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * dt;
        else if (input::is_key_down(key::a))
            camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * dt;
        else if (input::is_key_down(key::w))
            camera.position += camera.speed * camera.front * dt;
        else if (input::is_key_down(key::s))
            camera.position -= camera.speed * camera.front * dt;
        else if (input::is_key_down(key::SPACE))
            camera.position += camera.speed * camera.up * dt;
        else if (input::is_key_down(key::LSHIFT))
            camera.position -= camera.speed * camera.up * dt;
        if (input::is_key_down(key::UP))
            changeLightPosition.x += 0.1f;
        if (input::is_key_down(key::DOWN))
            changeLightPosition.x -= 0.1f;
        if (input::is_key_down(key::RIGHT))
            changeLightPosition.z += 0.1f;
        if (input::is_key_down(key::LEFT))
            changeLightPosition.z -= 0.1f;

        state->lights[0].position += changeLightPosition;
        // state->lights[0].direction = glm::normalize(state->lights[0].position - vec3(0));

        glm::vec2 mousePos = input::mouse_position();
        glm::vec2 offset{ mousePos.x - lastMousePos.x, lastMousePos.y - mousePos.y };

        lastMousePos = glm::vec2{ mousePos.x, mousePos.y };
        const f32 sensitivity = 0.04f;
        offset *= sensitivity;
        yaw += offset.x;
        pitch += offset.y;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera.front = glm::normalize(direction);

        if (input::is_left_mouse_down())
        {
            MouseEvent press_event(event_type::left_mouse_button_press, input::mouse_position());
            event_system::get()->post_all(event_type::left_mouse_button_press, &press_event);
        }
        MouseEvent move_event(event_type::mouse_move, input::mouse_position());
        event_system::get()->post_all(event_type::mouse_move, &move_event);

        camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    }
}

void application::close()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window->close();
}
