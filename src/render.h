#pragma once

struct RenderData
{
    Shader pbrShader;
    Shader depthShader;
    Shader depthNormalShader;
    Shader SSAOShader;
    Shader SSAOBlurShader;
    Shader equirectangularToCubemapShader;
    Shader backgroundShader;
};


void compileShader(const RenderData* renderData)
{
    renderData->pbrShader.compile("assets/shaders/Base.vs", "assets/shaders/Base.fs");
    renderData->depthShader.compile("assets/shaders/DepthShader.vs", "assets/shaders/DepthShader.fs");
    renderData->depthNormalShader.compile("assets/shaders/DepthNormalPass.vs", "assets/shaders/DepthNormalPass.fs");
    renderData->SSAOShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOPass.fs");
    renderData->SSAOBlurShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOBlurPass.fs");
    renderData->equirectangularToCubemapShader.compile("assets/shaders/Image2CubeMap.vs", "assets/shaders/Image2CubeMap.fs");
    renderData->backgroundShader.compile("assets/shaders/SkyBox.vs", "assets/shaders/SkyBox.fs");
}

void renderSetup(const RenderData* renderData)
{
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
}

