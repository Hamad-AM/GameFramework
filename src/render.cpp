#include "render.h"

#include <iostream>
#include <random>
#include <stb_image.h>

#include <glad/glad.h>

void CompileShaders(RenderData* renderData) {
    renderData->pbrShader.compile("assets/shaders/Base.vs", "assets/shaders/Base.fs");
    
    renderData->textureToScreen.compile("assets/shaders/textureToScreen.vs", "assets/shaders/textureToScreen.fs");

    renderData->shadowPass.depthShader.compile("assets/shaders/DepthShader.vs", "assets/shaders/DepthShader.fs");
    renderData->depthNormalShader.compile("assets/shaders/DepthNormalPass.vs", "assets/shaders/DepthNormalPass.fs");

    renderData->SSAOPass.SSAOShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOPass.fs");
    renderData->SSAOPass.SSAOBlurShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOBlurPass.fs");

    renderData->environmentMapPass.equirectangularToCubemapShader.compile("assets/shaders/Image2CubeMap.vs", "assets/shaders/Image2CubeMap.fs");
    renderData->environmentMapPass.backgroundShader.compile("assets/shaders/SkyBox.vs", "assets/shaders/SkyBox.fs");
    renderData->environmentMapPass.irradianceShader.compile("assets/shaders/IrradianceConvolution.vs", "assets/shaders/IrradianceConvolution.fs");
    renderData->environmentMapPass.prefilterShader.compile("assets/shaders/IrradianceConvolution.vs", "assets/shaders/Prefilter.fs");
    renderData->environmentMapPass.brdfShader.compile("assets/shaders/BrdfShader.vs", "assets/shaders/BrdfShader.fs");
    
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


void SetupShadowMapPass(RenderData* renderData, u32 shadow_width, u32 shadow_height)
{
    ShadowPass& pass = renderData->shadowPass;
    pass.width = shadow_width;
    pass.height = shadow_height;

    glGenFramebuffers(1, &pass.depthMapFBO);

    glGenTextures(1, &pass.depthMap);
    glBindTexture(GL_TEXTURE_2D, pass.depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        pass.width, pass.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, pass.depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pass.depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetupSSAOPass(RenderData* renderData)
{
    AmbientOcclusionPass& pass = renderData->SSAOPass;
    glGenFramebuffers(1, &pass.depthNormalPassBO);
    glBindFramebuffer(GL_FRAMEBUFFER, pass.depthNormalPassBO);

    // Create and attach depth texture
    glGenTextures(1, &pass.depthBuffer);
    glBindTexture(GL_TEXTURE_2D, pass.depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, renderData->render_width, renderData->render_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pass.depthBuffer, 0);

    // Create and attach normal texture
    glGenTextures(1, &pass.normalBuffer);
    glBindTexture(GL_TEXTURE_2D, pass.normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass.normalBuffer, 0);
    // Create and attach position texture
    glGenTextures(1, &pass.positionBuffer);
    glBindTexture(GL_TEXTURE_2D, pass.positionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderData->render_width, renderData->render_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pass.positionBuffer, 0);

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
    u32 hdrTexture = LoadSkyBoxTexture("assets/hdr/photo_studio_loft_hall_2k.hdr");

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

void RenderDepthNormalPass(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera)
{
    StartDepthNormalPass(renderData);
    UseDepthNormalShader(renderData, model, camera);
    DrawScene(renderData, meshRenderData);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSSAOPass(RenderData* renderData, Camera3D& camera)
{
    AmbientOcclusionPass& pass = renderData->SSAOPass;
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pass.depthBuffer);
    //SSAOShader.uniform_int("depthBuffer", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pass.normalBuffer);
    pass.SSAOShader.uniform_int("normalBuffer", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, pass.positionBuffer);
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


void RenderShadowMapPass(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, glm::vec3 lightPosition)
{
    ShadowPass& pass = renderData->shadowPass;
    
    glEnable(GL_DEPTH_TEST);
    
    //glCullFace(GL_FRONT);
    
    glViewport(0, 0, pass.width, pass.height);
    //glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, pass.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    pass.depthShader.bind();
    float near_plane = 0.1f, far_plane = 500.0f;
    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPosition,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    pass.lightSpaceMatrix = lightProjection * lightView;
    pass.depthShader.uniform_matrix4("lightSpaceMatrix", pass.lightSpaceMatrix);

    pass.depthShader.uniform_matrix4("model", model);

    DrawScene(renderData, meshRenderData);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
}

void RenderScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera, glm::vec3 lightPosition)
{
    glm::vec3 lightDirection = lightPosition - glm::vec3(0.0f);

    // SetupEnvironmentCubeMap(renderData);

    glViewport(0, 0, renderData->render_width, renderData->render_height);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderEnvironmentMap(renderData, camera);

    renderData->pbrShader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderData->shadowPass.depthMap);
    renderData->pbrShader.uniform_int("shadowMap", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderData->SSAOPass.ssaoColorBufferBlur);
    renderData->pbrShader.uniform_int("ssao", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->environmentMapPass.irradianceMap);
    renderData->pbrShader.uniform_int("irradianceMap", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderData->environmentMapPass.prefilterMap);
    renderData->pbrShader.uniform_int("prefilterMap", 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, renderData->environmentMapPass.brdfLUTTexture);
    renderData->pbrShader.uniform_int("brdfLUT", 4);
    renderData->pbrShader.uniform_matrix4("lightSpaceMatrix", renderData->shadowPass.lightSpaceMatrix);

    renderData->pbrShader.uniform_matrix4("model", model);
    renderData->pbrShader.uniform_matrix4("view", camera.view);
    renderData->pbrShader.uniform_matrix4("projection", camera.projection);
    renderData->pbrShader.uniform_vector3f("lightPosition", lightPosition);
    renderData->pbrShader.uniform_vector3f("cameraPosition", camera.position);

    for (u32 i = 0; i < meshRenderData.size(); ++i)
    {
        //if (sponza.meshes[i].name.find("decals") != std::string::npos)
        //{
        //    renderAfter = i;
        //}
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].albedoID);
        renderData->pbrShader.uniform_int("albedo", 5);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].normalMapID);
        renderData->pbrShader.uniform_int("normalMap", 6);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].metallicRoughnessID);
        renderData->pbrShader.uniform_int("metallicRoughness", 7);

        glBindVertexArray(meshRenderData[i].VAO);
        glDrawElements(GL_TRIANGLES, meshRenderData[i].numberOfIndices, GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
    }
}

void RenderEnvironmentMap(RenderData* renderData, Camera3D& camera)
{
    // glDisable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    //glDepthFunc(GL_LEQUAL);
    EnvironmentMapPass& pass = renderData->environmentMapPass;
    pass.backgroundShader.bind();
    pass.backgroundShader.uniform_matrix4("view", camera.view);
    pass.backgroundShader.uniform_matrix4("projection", camera.projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pass.envCubemap);
    RenderCube(renderData);
    // glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
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
    glBindVertexArray(0);}
