#include "render.h"

#include <iostream>
#include <random>
#include <stb_image.h>

#include <glad/glad.h>

void CompileShaders(RenderData* renderData) {
    renderData->forwardPass.pbrShader.compile("assets/shaders/Base.vs", "assets/shaders/Base.fs");
    
    renderData->textureToScreen.compile("assets/shaders/textureToScreen.vs", "assets/shaders/textureToScreen.fs");

    renderData->shadowPass.depthShader.compile("assets/shaders/CSMShader.vs", "assets/shaders/CSMShader.fs", "assets/shaders/CSMShader.gs");
    renderData->shadowPass.pointDepthShader.compile("assets/shaders/PointDepthShader.vs", "assets/shaders/PointDepthShader.fs", "assets/shaders/PointDepthShader.gs");
    renderData->depthNormalShader.compile("assets/shaders/DepthNormalPass.vs", "assets/shaders/DepthNormalPass.fs");

    renderData->SSAOPass.SSAOShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOPass.fs");
    renderData->SSAOPass.SSAOBlurShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOBlurPass.fs");

    renderData->environmentMapPass.equirectangularToCubemapShader.compile("assets/shaders/Image2CubeMap.vs", "assets/shaders/Image2CubeMap.fs");
    renderData->environmentMapPass.backgroundShader.compile("assets/shaders/SkyBox.vs", "assets/shaders/SkyBox.fs");
    renderData->environmentMapPass.irradianceShader.compile("assets/shaders/IrradianceConvolution.vs", "assets/shaders/IrradianceConvolution.fs");
    renderData->environmentMapPass.prefilterShader.compile("assets/shaders/IrradianceConvolution.vs", "assets/shaders/Prefilter.fs");
    renderData->environmentMapPass.brdfShader.compile("assets/shaders/BrdfShader.vs", "assets/shaders/BrdfShader.fs");


    renderData->deferredPass.gBufferShader.compile("assets/shaders/GBuffer.vs", "assets/shaders/GBuffer.fs");
    renderData->deferredPass.lightingPass.compile("assets/shaders/LightingPass.vs", "assets/shaders/LightingPass.fs");

    renderData->lightProbePass.colorShader.compile("assets/shaders/RenderLightProbe.vs", "assets/shaders/RenderLightProbe.fs");

    renderData->UnlitShader.compile("assets/shaders/UnlitShader.vs", "assets/shaders/UnlitShader.fs");

    renderData->sphereCubemap.compile("assets/shaders/SphereCubemap.vs", "assets/shaders/SphereCubemap.fs");
}

void RenderSetupParameters(RenderData* renderData, u32 render_width, u32 render_height)
{
    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);
    //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    renderData->render_width = render_width;
    renderData->render_height = render_height;
}

void SetupGBuffers(RenderData* renderData)
{
    DeferredPass& pass = renderData->deferredPass;
    glGenFramebuffers(1, &pass.gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pass.gBuffer);

    // - position color buffer
    glGenTextures(1, &pass.gPosition);
    glBindTexture(GL_TEXTURE_2D, pass.gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass.gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &pass.gNormal);
    glBindTexture(GL_TEXTURE_2D, pass.gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pass.gNormal, 0);

    // - color buffer
    glGenTextures(1, &pass.gAlbedo);
    glBindTexture(GL_TEXTURE_2D, pass.gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, pass.gAlbedo, 0);

    glGenTextures(1, &pass.gMetallicRoughness);
    glBindTexture(GL_TEXTURE_2D, pass.gMetallicRoughness);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, pass.gMetallicRoughness, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    glGenRenderbuffers(1, &pass.rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, pass.rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderData->render_width, renderData->render_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pass.rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderGBuffer(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera)
{
    DeferredPass& pass = renderData->deferredPass;
    glViewport(0, 0, renderData->render_width, renderData->render_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, pass.gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pass.gBufferShader.bind();
    pass.gBufferShader.uniform_matrix4("projection", camera.projection);
    pass.gBufferShader.uniform_matrix4("view", camera.view);
    pass.gBufferShader.uniform_matrix4("model", model);
    for (u32 i = 0; i < meshRenderData.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].albedoID);
        pass.gBufferShader.uniform_int("albedo", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].normalMapID);
        pass.gBufferShader.uniform_int("normalMap", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].metallicRoughnessID);
        pass.gBufferShader.uniform_int("metallicRoughness", 2);

        glBindVertexArray(meshRenderData[i].VAO);
        glDrawElements(GL_TRIANGLES, meshRenderData[i].numberOfIndices, GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void RenderDeferredScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera)
{
    DeferredPass& pass = renderData->deferredPass;
    glViewport(0, 0, renderData->render_width, renderData->render_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderData->lightShaderStorageObject);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, renderData->shadowPass.lightMatricesUBO);

    pass.lightingPass.bind();
    pass.lightingPass.uniform_matrix4("view", camera.view);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pass.gPosition);
    pass.lightingPass.uniform_int("gPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pass.gNormal);
    pass.lightingPass.uniform_int("gNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, pass.gAlbedo);
    pass.lightingPass.uniform_int("gAlbedo", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, pass.gMetallicRoughness);
    pass.lightingPass.uniform_int("gMetallicRoughness", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderData->shadowPass.lightDepthmaps);
    pass.lightingPass.uniform_int("shadowMap", 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, renderData->SSAOPass.ssaoColorBufferBlur);
    pass.lightingPass.uniform_int("ssao", 5);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->lightProbePass.irradianceMap);
    pass.lightingPass.uniform_int("irradianceMap", 6);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->environmentMapPass.prefilterMap);
    pass.lightingPass.uniform_int("prefilterMap", 7);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, renderData->environmentMapPass.brdfLUTTexture);
    pass.lightingPass.uniform_int("brdfLUT", 8);
    for (int i = 0; i < renderData->shadowPass.pointCount; ++i)
    {
        glActiveTexture(GL_TEXTURE9+i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->shadowPass.pointShadows[i].depthCubemap);
        std::string name = "depthCubemaps[" + std::to_string(i) + "]";
        pass.lightingPass.uniform_int(name.c_str(), 9+i);
    }
    pass.lightingPass.uniform_float("pointShadowFarPlane", renderData->shadowPass.farPlane);
    pass.lightingPass.uniform_vector3f("viewPos", camera.position);
    pass.lightingPass.uniform_int("cascadeCount", renderData->shadowPass.cascadesCount);
    for (s32 i = 0; i < renderData->shadowPass.cascadesCount-1; ++i)
    {
        std::string uniformName = "cascadePlaneDistances[" + std::to_string(i) + "]";
        pass.lightingPass.uniform_float(uniformName.c_str(), renderData->shadowPass.shadowCascadeLevels[i]);
    }

    RenderQuad(renderData);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, pass.gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, renderData->render_width, renderData->render_height, 0, 0, renderData->render_width, renderData->render_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderData->UnlitShader.bind();
    renderData->UnlitShader.uniform_matrix4("projection", camera.projection);
    renderData->UnlitShader.uniform_matrix4("view", camera.view);
    for (s32 i = 0; i < renderData->numLights; ++i)
    {
        glm::mat4 lightModel(1.0f);
        lightModel = glm::translate(lightModel, renderData->lights[i].position);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f));
        renderData->UnlitShader.uniform_matrix4("model", lightModel);
        renderData->UnlitShader.uniform_vector3f("lightColor", renderData->lights[i].color);
        RenderCube(renderData);
    }

    renderData->sphereCubemap.bind();
    renderData->sphereCubemap.uniform_matrix4("projection", camera.projection);
    renderData->sphereCubemap.uniform_matrix4("view", camera.view);
    glm::mat4 lightModel(1.0f);
    lightModel = glm::translate(lightModel, renderData->lightProbePass.position);
    lightModel = glm::scale(lightModel, glm::vec3(0.1f));
    renderData->sphereCubemap.uniform_matrix4("model", lightModel);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->lightProbePass.irradianceMap);
    renderData->sphereCubemap.uniform_int("cubemap", 0);
    RenderSphere(renderData);

    RenderEnvironmentMap(renderData, camera);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SetupShadowMapPass(RenderData* renderData, Light* lights, u32 numLights, u32 shadow_width, u32 shadow_height, Camera3D& camera)
{
    ShadowPass& pass = renderData->shadowPass;

    pass.depthMapResolution = 2048;

    glGenFramebuffers(1, &pass.depthMapFBO);

    glGenTextures(1, &pass.lightDepthmaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, pass.lightDepthmaps);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
        pass.depthMapResolution, pass.depthMapResolution, int(pass.cascadesCount), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, pass.depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pass.lightDepthmaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &pass.lightMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, pass.lightMatricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, pass.lightMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SetupPointShadowMapPass(RenderData* renderData, Light* lights, u32 numLights, u32 shadow_width, u32 shadow_height)
{
    ShadowPass& pass = renderData->shadowPass;
    
    glGenFramebuffers(1, &pass.depthCubemapFBO);

    for (u32 i = 0; i < numLights; ++i)
    {
        if (pass.pointCount < 4 && lights[i].type == LightType::Point)
        {
            // TODO(HAMAD): have different shadow resolutions for different lights
            pass.pointShadows[pass.pointCount].pointWidth = shadow_width;
            pass.pointShadows[pass.pointCount].pointHeight = shadow_height;
            glGenTextures(1, &pass.pointShadows[pass.pointCount].depthCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, pass.pointShadows[pass.pointCount].depthCubemap);
            for (u32 j = 0; j < 6; ++j)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT,
                    pass.pointShadows[pass.pointCount].pointWidth, pass.pointShadows[pass.pointCount].pointHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindFramebuffer(GL_FRAMEBUFFER, pass.depthCubemapFBO);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pass.pointShadows[pass.pointCount].depthCubemap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            lights[i].pointShadowMapIndex = pass.pointCount;
            pass.pointCount += 1;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderOmidirectionalShadowMap(RenderData* renderData,std::vector<MeshRenderData>& meshRenderData, mat4& model, Light* lights, u32 numLights)
{
    ShadowPass& pass = renderData->shadowPass;
    f32 nearPlane = 1.0f;
    f32 farPlane = 15.0f;
    pass.farPlane = farPlane;
    std::vector<glm::mat4> shadowTransforms;
    vec3 lightPos;
    for (u32 i = 0; i < numLights; ++i)
    {
        if (lights[i].type == LightType::Point)
        {
            Light* light = &lights[i];
            PointLightShadow& point = pass.pointShadows[light->pointShadowMapIndex];
            mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (f32)point.pointWidth/(f32)point.pointHeight, nearPlane, farPlane);
            point.farPlane = farPlane;
            //shadowProjection[1][1] *= -1;
            lightPos = light->position;
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

            glViewport(0, 0,point.pointWidth,point.pointHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, pass.depthCubemapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            pass.pointDepthShader.bind();
            pass.pointDepthShader.uniform_float("far_plane", point.farPlane);
            pass.pointDepthShader.uniform_vector3f("lightPos", lightPos);
            for (unsigned int j = 0; j < 6; ++j)
            {
                std::string uniform = "shadowMatrices[" + std::to_string(j) + "]";
                pass.pointDepthShader.uniform_matrix4(uniform.c_str(), shadowTransforms[j]);
            }
            pass.pointDepthShader.uniform_matrix4("model", model);
            DrawScene(renderData, meshRenderData);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetupLightProbe(RenderData* renderData)
{
    LightProbePass& pass = renderData->lightProbePass;
    glGenFramebuffers(1, &pass.FBO);
    // glGenRenderbuffers(1, &pass.captureRBO);

    pass.width = 512;
    pass.height = 512;
    glBindFramebuffer(GL_FRAMEBUFFER, pass.FBO);
    glGenTextures(1, &pass.cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.cubemap);
    for (u32 j = 0; j < 6; ++j)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_RGBA16F,
            pass.width, pass.height, 0, GL_RGBA, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // glBindFramebuffer(GL_FRAMEBUFFER, pass.FBO);
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pass.cubemap, 0);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderLightProbe(RenderData* renderData, vec3& lightProbePosition, std::vector<MeshRenderData>& meshRenderData, mat4& model, Light* lights, u32 numOfLights)
{
    LightProbePass& pass = renderData->lightProbePass;
    pass.position = lightProbePosition;
    f32 nearPlane = 1.0f;
    f32 farPlane = 15.0f;
    mat4 proj = glm::perspective(glm::radians(90.0f), (f32)pass.width/(f32)pass.height, nearPlane, farPlane);
    mat4 probeTransforms[] = {
        proj * glm::lookAt(lightProbePosition, lightProbePosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj * glm::lookAt(lightProbePosition, lightProbePosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj * glm::lookAt(lightProbePosition, lightProbePosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        proj * glm::lookAt(lightProbePosition, lightProbePosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        proj * glm::lookAt(lightProbePosition, lightProbePosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        proj * glm::lookAt(lightProbePosition, lightProbePosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };
    // TODO: Fix this alternative
    glm::mat4 inv = glm::inverse(proj);

    glViewport(0, 0, pass.width, pass.height);
    glBindFramebuffer(GL_FRAMEBUFFER, pass.FBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderData->lightShaderStorageObject);
    for (u32 i = 0; i < 6; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pass.cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // TODO: FIx this hack
        mat4 view = inv * probeTransforms[i];
        RenderEnvironmentMap(renderData, view, proj);

        pass.colorShader.bind();
        pass.colorShader.uniform_matrix4("model", model);
        pass.colorShader.uniform_matrix4("viewProj", probeTransforms[i]);
        for (u32 j = 0; j < meshRenderData.size(); ++j)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, meshRenderData[j].albedoID);
            pass.colorShader.uniform_int("albedo", 0);

            glBindVertexArray(meshRenderData[j].VAO);
            glDrawElements(GL_TRIANGLES, meshRenderData[j].numberOfIndices, GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
        }
    }


    
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.cubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };


    glGenTextures(1, &pass.irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    renderData->environmentMapPass.irradianceShader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.cubemap);
    renderData->environmentMapPass.irradianceShader.uniform_int("environmentMap", 0);

    glCullFace(GL_FRONT);
    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, pass.FBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        renderData->environmentMapPass.irradianceShader.uniform_matrix4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pass.irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube(renderData);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
}

void SetupSSAOPass(RenderData* renderData)
{
    AmbientOcclusionPass& pass = renderData->SSAOPass;

    glGenFramebuffers(1, &pass.ssaoFBO);  glGenFramebuffers(1, &pass.ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, pass.ssaoFBO);
    // SSAO color buffer
    glGenTextures(1, &pass.ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, pass.ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass.ssaoColorBuffer, 0);
    GLenum attachmentsSSAO[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachmentsSSAO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, pass.ssaoBlurFBO);
    glGenTextures(1, &pass.ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, pass.ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass.ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3>& ssaoKernel = pass.ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &pass.noiseTexture);
    glBindTexture(GL_TEXTURE_2D, pass.noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SetupEnvironmentCubeMap(RenderData* renderData)
{
    u32 hdrTexture = LoadSkyBoxTexture("assets/hdr/rogland_overcast_4k.hdr");

    EnvironmentMapPass& pass = renderData->environmentMapPass;

    // glDisable(GL_DEPTH_TEST);

    glGenFramebuffers(1, &pass.captureFBO);
    glGenRenderbuffers(1, &pass.captureRBO);

    SetupBRDFLUT(renderData, pass);

    glBindFramebuffer(GL_FRAMEBUFFER, pass.captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, pass.captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pass.captureRBO);

    glGenTextures(1, &pass.envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    pass.equirectangularToCubemapShader.bind();
    pass.equirectangularToCubemapShader.uniform_int("equirectangularMap", 0);
    pass.equirectangularToCubemapShader.uniform_matrix4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glCullFace(GL_FRONT);
    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, pass.captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        pass.equirectangularToCubemapShader.uniform_matrix4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pass.envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube(renderData); // renders a 1x1 cube
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glGenTextures(1, &pass.irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, pass.captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, pass.captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    
    pass.irradianceShader.bind();
    pass.irradianceShader.uniform_int("environmentMap", 0);
    pass.irradianceShader.uniform_matrix4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.envCubemap);

    glCullFace(GL_FRONT);
    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, pass.captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        pass.irradianceShader.uniform_matrix4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pass.irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube(renderData);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &pass.prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    pass.prefilterShader.bind();
    pass.prefilterShader.uniform_int("environmentMap", 0);
    pass.prefilterShader.uniform_matrix4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, pass.captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, pass.captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        pass.prefilterShader.uniform_float("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            pass.prefilterShader.uniform_matrix4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pass.prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderCube(renderData);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);

}

void SetupLightsBuffer(RenderData* renderData, Light* lights, u32 numLights)
{
    glGenBuffers(1, &renderData->lightShaderStorageObject);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderData->lightShaderStorageObject);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numLights * sizeof(Light), lights, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderData->lightShaderStorageObject); // Bind to binding point 0
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
    renderData->lights = lights;
    renderData->numLights = numLights;
}

void SetupBRDFLUT(RenderData* renderData, EnvironmentMapPass& pass)
{
    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    glGenTextures(1, &pass.brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.3
    glBindTexture(GL_TEXTURE_2D, pass.brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    //
    glBindFramebuffer(GL_FRAMEBUFFER, pass.captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, pass.captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass.brdfLUTTexture, 0);

    // glDisable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);
    // glCullFace(GL_BACK);
    
    glDisable(GL_BLEND);

    glViewport(0, 0, 512, 512);
    pass.brdfShader.bind();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderQuad(renderData);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderBRDFLUT(RenderData* renderData)
{
    EnvironmentMapPass& pass = renderData->environmentMapPass;

    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderData->textureToScreen.bind();    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pass.brdfLUTTexture);
    renderData->textureToScreen.uniform_int("uTexture", 0);
    RenderQuad(renderData);
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
}


u32 LoadSkyBoxTexture(const char* filePath)
{
    stbi_set_flip_vertically_on_load(true);

    s32 width, height, channels;
    f32* data = stbi_loadf(filePath, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load HDR image from " << filePath << std::endl;
    }

    GLuint hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return hdrTexture;
}


void StartDepthNormalPass(RenderData* renderData)
{
    glViewport(0, 0, renderData->render_width, renderData->render_height);
    glBindFramebuffer(GL_FRAMEBUFFER, renderData->SSAOPass.depthNormalPassBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void UseDepthNormalShader(RenderData* renderData, glm::mat4 model, Camera3D& camera)
{
    renderData->depthNormalShader.bind();
    renderData->depthNormalShader.uniform_matrix4("model", model);
    renderData->depthNormalShader.uniform_matrix4("view", camera.view);
    renderData->depthNormalShader.uniform_matrix4("projection", camera.projection);
}

void DrawScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData)
{
    for (u32 i = 0; i < meshRenderData.size(); ++i)
    {
        glBindVertexArray(meshRenderData[i].VAO);
        glDrawElements(GL_TRIANGLES, meshRenderData[i].numberOfIndices, GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
    }
}


void RenderSSAOPass(RenderData* renderData, Camera3D& camera)
{
    AmbientOcclusionPass& pass = renderData->SSAOPass;
    glViewport(0, 0, renderData->render_width, renderData->render_height);
    pass.SSAOShader.bind();
    glBindFramebuffer(GL_FRAMEBUFFER, pass.ssaoFBO);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    // Send kernel + rotation 
    for (unsigned int i = 0; i < 64; ++i)
    {
        std::string kernelSample = "samples[" + std::to_string(i) + "]";
        pass.SSAOShader.uniform_vector3f(kernelSample.c_str(), pass.ssaoKernel[i]);
    }
    pass.SSAOShader.uniform_matrix4("projection", camera.projection);
    pass.SSAOShader.uniform_matrix4("view", camera.view);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderData->deferredPass.gNormal);
    pass.SSAOShader.uniform_int("normalBuffer", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, renderData->deferredPass.gPosition);
    pass.SSAOShader.uniform_int("positionBuffer", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, pass.noiseTexture);
    pass.SSAOShader.uniform_int("texNoise", 3);
    GLuint program = pass.SSAOShader.program();
    // glDisable(GL_DEPTH_TEST);//remove this
    RenderQuad(renderData);
    // glEnable(GL_DEPTH_TEST);//remove this
//
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, pass.ssaoBlurFBO);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    pass.SSAOBlurShader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pass.ssaoColorBuffer);
    pass.SSAOBlurShader.uniform_int("ssaoInput", 0);
    RenderQuad(renderData);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderShadowMapPass(RenderData* renderData, vec3& lightDirection, std::vector<MeshRenderData>& meshRenderData, glm::mat4& model, Camera3D& camera)
{
    ShadowPass& pass = renderData->shadowPass;

    const u32 numberOfCascades = 4;
    pass.shadowCascadeLevels[0] = camera.farPlane / 20.0f;
    pass.shadowCascadeLevels[1] = (camera.farPlane * 3) / 20.0f;
    pass.shadowCascadeLevels[2] = (camera.farPlane * 2) / 5.0f;
    // float lambda = 0.8f;
    // for (int i = 0; i < numberOfCascades - 1; ++i)
    // {
    //     float linearSplit = camera.nearPlane + (camera.farPlane - camera.nearPlane) * (i + 1) / numberOfCascades;
    //     float logSplit = camera.nearPlane * pow((camera.farPlane / camera.nearPlane), (float)(i + 1) / numberOfCascades);
    //     pass.shadowCascadeLevels[i] = lambda * logSplit + (1.0f - lambda) * linearSplit;
    // }
    mat4 lightSpaceMatrices[numberOfCascades];
    CalculateCascadesLightSpaceMatrices(lightSpaceMatrices, pass.shadowCascadeLevels, numberOfCascades, lightDirection, camera);
    glBindBuffer(GL_UNIFORM_BUFFER, pass.lightMatricesUBO);
    for (size_t i = 0; i < numberOfCascades; ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightSpaceMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    
    //glCullFace(GL_FRONT);
    glDisable(GL_CULL_FACE);
    
    glViewport(0, 0, pass.depthMapResolution, pass.depthMapResolution);
    // glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, pass.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindBuffer(GL_UNIFORM_BUFFER, pass.lightMatricesUBO);
    pass.depthShader.bind();
    pass.depthShader.uniform_matrix4("model", model);
    for (u32 i = 0; i < meshRenderData.size(); ++i)
    {
        glBindVertexArray(meshRenderData[i].VAO);
        glDrawElements(GL_TRIANGLES, meshRenderData[i].numberOfIndices, GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void RenderScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera, glm::vec3 lightPosition)
{
    glm::vec3 lightDirection = lightPosition - glm::vec3(0.0f);

    // SetupEnvironmentCubeMap(renderData);

    glViewport(0, 0, renderData->render_width, renderData->render_height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderEnvironmentMap(renderData, camera);

    renderData->forwardPass.pbrShader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderData->shadowPass.lightDepthmaps);
    renderData->forwardPass.pbrShader.uniform_int("shadowMap", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderData->SSAOPass.ssaoColorBufferBlur);
    renderData->forwardPass.pbrShader.uniform_int("ssao", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->environmentMapPass.irradianceMap);
    renderData->forwardPass.pbrShader.uniform_int("irradianceMap", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->environmentMapPass.prefilterMap);
    renderData->forwardPass.pbrShader.uniform_int("prefilterMap", 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, renderData->environmentMapPass.brdfLUTTexture);
    renderData->forwardPass.pbrShader.uniform_int("brdfLUT", 4);
    renderData->forwardPass.pbrShader.uniform_matrix4("lightSpaceMatrix", renderData->shadowPass.lightSpaceMatrix);
    renderData->forwardPass.pbrShader.uniform_vector3f("lightPosition", lightPosition);
    renderData->forwardPass.pbrShader.uniform_vector3f("cameraPosition", camera.position);

    renderData->forwardPass.pbrShader.uniform_matrix4("model", model);
    renderData->forwardPass.pbrShader.uniform_matrix4("view", camera.view);
    renderData->forwardPass.pbrShader.uniform_matrix4("projection", camera.projection);

    for (u32 i = 0; i < meshRenderData.size(); ++i)
    {
        //if (sponza.meshes[i].name.find("decals") != std::string::npos)
        //{
        //    renderAfter = i;
        //}
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].albedoID);
        renderData->forwardPass.pbrShader.uniform_int("albedo", 5);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].normalMapID);
        renderData->forwardPass.pbrShader.uniform_int("normalMap", 6);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].metallicRoughnessID);
        renderData->forwardPass.pbrShader.uniform_int("metallicRoughness", 7);

        glBindVertexArray(meshRenderData[i].VAO);
        glDrawElements(GL_TRIANGLES, meshRenderData[i].numberOfIndices, GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
    }
}

void RenderEnvironmentMap(RenderData* renderData, Camera3D& camera)
{
    RenderEnvironmentMap(renderData, camera.view, camera.projection);
}

void RenderEnvironmentMap(RenderData* renderData, mat4& view, mat4& projection)
{
    // glDisable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    //glDepthFunc(GL_LEQUAL);
    EnvironmentMapPass& pass = renderData->environmentMapPass;
    pass.backgroundShader.bind();
    mat4 viewProj = projection * mat4(glm::mat3(view));
    pass.backgroundShader.uniform_matrix4("viewProj", viewProj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.irradianceMap);
    pass.backgroundShader.uniform_int("environmentMap", 0);
    RenderCube(renderData);
    // glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
}

void getFrustumCornersWorldSpace(glm::vec4* frustumCorners, const glm::mat4& proj, const glm::mat4& view)
{
    const auto inv = glm::inverse(proj * view);

    u32 index = 0;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt =
                    inv * glm::vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f);
                frustumCorners[index++] = (pt / pt.w);
            }
        }
    }
}

mat4 GetLightSpaceMatrix(Camera3D& camera, vec3& lightDirection, f32 nearPlane, f32 farPlane)
{
    glm::vec4 corners[8] = {};
    mat4 projection = glm::perspective(glm::radians(camera.fov), camera.ratio, nearPlane, farPlane);
    getFrustumCornersWorldSpace(corners, projection, camera.view);
    vec3 center = vec3(0, 0, 0);
    for (vec3 corner : corners)
    {
        center += vec3(corner);
    }
    center /= 8;

    const mat4 lightView = glm::lookAt(center + lightDirection, center, vec3(0.0, 1.0, 0.0));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }
    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

void CalculateCascadesLightSpaceMatrices(mat4* matrices, f32* shadowCascadeLevels, u32 numberOfCascades, vec3& lightDirection, Camera3D& camera)
{
    for (u32 i = 0; i < numberOfCascades; ++i)
    {
        if (i == 0)
        {
            matrices[i] = GetLightSpaceMatrix(camera, lightDirection, camera.nearPlane, shadowCascadeLevels[i]);
        }
        else if (i < numberOfCascades-1)
        {
            matrices[i] = GetLightSpaceMatrix(camera, lightDirection, shadowCascadeLevels[i - 1], shadowCascadeLevels[i]);
        }
        else
        {
            matrices[i] = GetLightSpaceMatrix(camera, lightDirection, shadowCascadeLevels[i - 1], camera.farPlane);
        }
    }
}

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void RenderCube(RenderData* renderData)
{
    // initialize (if necessary)
    if (renderData->cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        GLuint cubeVBO;
        glGenVertexArrays(1, &renderData->cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(renderData->cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(renderData->cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void RenderQuad(RenderData* renderData)
{
    if (renderData->quadVAO == 0)
    {
        GLuint quadVBO;
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &renderData->quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(renderData->quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(renderData->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderSphere(RenderData* renderData)
{
    if (renderData->sphereVAO == 0)
    {
        glGenVertexArrays(1, &renderData->sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        renderData->sphereIndexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(renderData->sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(renderData->sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, renderData->sphereIndexCount, GL_UNSIGNED_INT, 0);
}
