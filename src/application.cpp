#include "common.h"

#include "application.h"
#include "input.h"
#include "components.h"

#include <iostream>
#include <map>
#include <limits>
#include <chrono>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <random>

namespace atl
{
application::application() {}

application::~application()
{
    delete window;
}

void application::initialize(u32 screen_width, u32 screen_height)
{

    window = new glfw_window();
    screen_width_ = screen_width;
    screen_height_ = screen_height;
    window->initialize(screen_width_, screen_height_, "Game");

    camera2d_position = vec3(0.0f, 0.0f, 0.0f);
    camera2d.set_projection(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f));


    sponza = load_model("assets/Sponza/sponza.glb");

    BatchModel(sponza);
    for (RenderData& renderData : meshRenderData)
    {
        UploadDataToGL(renderData);
    }
    
    shader.compile("assets/shaders/Base.vs", "assets/shaders/Base.fs");
    depthShader.compile("assets/shaders/DepthShader.vs", "assets/shaders/DepthShader.fs");
    depthNormalShader.compile("assets/shaders/DepthNormalPass.vs", "assets/shaders/DepthNormalPass.fs");
    SSAOShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOPass.fs");
    SSAOBlurShader.compile("assets/shaders/SSAOPass.vs", "assets/shaders/SSAOBlurPass.fs");

    //render.initialize();

    audio.init();

//     for (u32 i = 0; i < entities.size(); ++i)
//     {
//         entities[i]->init(&render, &audio);
//     }
}
 
void application::run()
{
    previous_time = window->get_time();
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_MULTISAMPLE);
    //glDisable(GL_CULL_FACE);
    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(1.0f, 1.0f);

    camera = {};
    camera.position = glm::vec3(0.1f, 0.1f, 0.1f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.projection = glm::perspective(glm::radians(90.0f), (f32)screen_width_ / screen_height_, 0.01f, 100.0f);
    camera.orientation = glm::quat(glm::vec3(0.f, 0.f, 0.f));


    camera.front = glm::vec3(0.0, 0.0, -1.0);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.right = glm::normalize(glm::cross(camera.up, camera.front));
    camera.speed = 0.5f;
    camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

    lastMousePos = glm::vec2(0.0f);

    glGenFramebuffers(1, &depthMapFBO);

    const u32 SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint depthBuffer, normalBuffer, depthNormalPassBO;
    glGenFramebuffers(1, &depthNormalPassBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthNormalPassBO);

    // Create and attach depth texture
    glGenTextures(1, &depthBuffer);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screen_width_, screen_height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

    // Create and attach normal texture
    glGenTextures(1, &normalBuffer);
    glBindTexture(GL_TEXTURE_2D, normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_, screen_height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalBuffer, 0);

    GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // also create framebuffer to hold SSAO processing stage 
    // -----------------------------------------------------
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width_, screen_height_, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width_, screen_height_, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ourLerp(0.1f, 1.0f, scale * scale);
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
    unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    lightPosition = glm::vec3(1.0f, 5.0f, 1.0f);

    while (!window->should_close())
    {
        window->handle_input();
        // state.audio.update();

        f64 current_time = window->get_time();
        f64 dt = current_time - previous_time;
        previous_time = current_time;

        if (dt < 0)
            dt = 0.0001;

        glViewport(0, 0, screen_width_, screen_height_);
        glBindFramebuffer(GL_FRAMEBUFFER, depthNormalPassBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        depthNormalShader.bind();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.001));

        depthNormalShader.bind();
        depthNormalShader.uniform_matrix4("model", model);
        camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
        depthNormalShader.uniform_matrix4("view", camera.view);
        depthNormalShader.uniform_matrix4("projection", camera.projection);

        for (u32 i = 0; i < meshRenderData.size(); ++i)
        {
            glBindVertexArray(meshRenderData[i].VAO);
            glDrawElements(GL_TRIANGLES, meshRenderData[i].indices.size(), GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
            SSAOShader.bind();
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            // NOTE: REMOVE THIS
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            // Send kernel + rotation 
            for (unsigned int i = 0; i < 64; ++i)
            {
                std::string kernelSample = "samples[" + std::to_string(i) + "]";
                SSAOShader.uniform_vector3f(kernelSample.c_str(), ssaoKernel[i]);
            }
            SSAOShader.uniform_matrix4("projection", camera.projection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthBuffer);
            SSAOShader.uniform_int("depthBuffer", 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalBuffer);
            SSAOShader.uniform_int("normalBuffer", 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            SSAOShader.uniform_int("texNoise", 2);
            glDisable(GL_DEPTH_TEST);//remove this
            renderQuad();
            glEnable(GL_DEPTH_TEST);//remove this
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if 0
        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            SSAOBlurShader.bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            SSAOBlurShader.uniform_int("ssaoInput", 0);
            renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glCullFace(GL_FRONT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.bind();
        float near_plane = 1.0f, far_plane = 10.0f;
        glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPosition,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
        depthShader.uniform_matrix4("lightSpaceMatrix", lightSpaceMatrix);

        depthShader.uniform_matrix4("model", model);
        
        for (u32 i = 0; i < meshRenderData.size(); ++i)
        {
            glBindVertexArray(meshRenderData[i].VAO);
            glDrawElements(GL_TRIANGLES, meshRenderData[i].indices.size(), GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCullFace(GL_BACK);
        
        shader.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader.uniform_int("shadowMap", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        shader.uniform_int("ssao", 1);
        shader.uniform_matrix4("lightSpaceMatrix", lightSpaceMatrix);

        update(dt);
#endif

        window->swap_buffers();
    }
}

void application::update(f32 dt)
{
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
    else if (input::is_key_down(key::UP))
        lightPosition.x += 1.0f;
    else if (input::is_key_down(key::DOWN))
        lightPosition.x -= 1.0f;
    else if (input::is_key_down(key::RIGHT))
        lightPosition.y += 1.0f;
    else if (input::is_key_down(key::LEFT))
        lightPosition.y -= 1.0f;
    else if (input::is_key_down(key::p))
        return;

    glm::vec2 mousePos = input::mouse_position();
    glm::vec2 offset{ mousePos.x - lastMousePos.x, lastMousePos.y - mousePos.y };

    lastMousePos = glm::vec2{ mousePos.x, mousePos.y };
    const f32 sensitivity = 0.075f;
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
        mouse_event press_event{event_type::left_mouse_button_press, input::mouse_position()};
        event_system::get()->post_all(event_type::left_mouse_button_press, &press_event);
    }
    mouse_event move_event{event_type::mouse_move, input::mouse_position()};
    event_system::get()->post_all(event_type::mouse_move, &move_event);

    //glm::mat4 view = glm::mat4(1.0f);
    //view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    //std::cout << camera.position.x << " " << camera.position.y <<  " " << camera.position.z << std::endl;
    glViewport(0, 0, screen_width_, screen_height_);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.001));
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    shader.uniform_matrix4("model", model);
    camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    shader.uniform_matrix4("view", camera.view);
    shader.uniform_matrix4("projection", camera.projection);
    glm::vec3 lightDirection = lightPosition - glm::vec3(0.0f);
    shader.uniform_vector3f("lightDir", lightDirection);

    u32 renderAfter = 0;
    for (u32 i = 0; i < meshRenderData.size(); ++i)
    {
        //if (sponza.meshes[i].name.find("decals") != std::string::npos)
        //{
        //    renderAfter = i;
        //}
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].albedoID);
        shader.uniform_int("albedo", 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, meshRenderData[i].normalMapID);
        shader.uniform_int("normalMap", 3);

        glBindVertexArray(meshRenderData[i].VAO);
        glDrawElements(GL_TRIANGLES, meshRenderData[i].indices.size(), GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
    }

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, meshRenderData[renderAfter].albedoID);
    //shader.uniform_int("albedo", 0);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, meshRenderData[renderAfter].normalMapID);
    //shader.uniform_int("normalMap", 1);

    //glBindVertexArray(meshRenderData[renderAfter].VAO);
    //glDrawElements(GL_TRIANGLES, meshRenderData[renderAfter].indices.size(), GL_UNSIGNED_INT, (void*)(0 * sizeof(u32)));
    //glDrawElements(GL_TRIANGLES, renderData.meshOffsets[0].indexCount, GL_UNSIGNED_INT, (void*)(renderData.meshOffsets[0].indexOffset * sizeof(u32)));

    
//     camera2d.update(camera_position);
//
//     for (u32 i = 0; i < entities.size(); ++i)
//     {
//         entities[i]->update(dt);
//     }
//
//     render.begin2d(camera2d);
//
//     vec4 color{ 0.9, 0.9, 0.9, 1.0f };
//     render.draw_text(std::to_string((u32)round(dt*1000)) + " ms", screen_width_*0.9, screen_height_*0.95, 0.5, font_type::Montserrat, color, &camera2d);
//     // std::string memory_str = std::to_string(allocated_memory/1000) + " MB";
//     // state.render.draw_text(memory_str, screen_width_*0.875, screen_height_*0.9, 0.5, font_type::Montserrat, color, &state.camera2d);
//
//     for (u32 i = 0; i < entities.size(); ++i)
//     {
//         ref<entity> e = entities[i];
//         if (e->get_sprite() != nullptr)
//         {
//             vec2 p(e->get_transform().position.x, e->get_transform().position.y);
//             render.draw_texture(e->get_sprite()->texture, p, e->get_sprite()->size, e->get_transform().rotation.z, e->get_sprite()->color, e->get_sprite()->tiling_factor, &camera2d);
//         }
//     }
// 
//     render.end2d();
}

void application::close()
{

    render.render_delete();
    window->close();
}
Model application::load_model(std::string path)
{
    Model result;
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, "assets/Sponza/sponza.glb");

    if (ret)
    {
        for (const auto& glmesh : model.meshes) {
            for (const auto& primitive : glmesh.primitives) {
                Mesh mesh;

                bool calculateTangents = false;
                mesh.name = glmesh.name;
                if (mesh.name.find("decals") != std::string::npos) {
                    continue;
                }

                // Get vertex data
                const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
                const float* positions = reinterpret_cast<const float*>(&model.buffers[positionBufferView.buffer].data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

                for (size_t i = 0; i < positionAccessor.count; ++i)
                {
                    mesh.positions.emplace_back(glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]));
                }

                // Get normal data if available
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
                    const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
                    const float* normals = reinterpret_cast<const float*>(&model.buffers[normalBufferView.buffer].data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
                    for (size_t i = 0; i < normalAccessor.count; ++i)
                    {
                        mesh.normals.emplace_back(glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]));
                    }
                }
                else {
                    assert(("Mesh has no Normals", false));
                }

                if (primitive.attributes.count("TEXCOORD_0")) {
                    const auto& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                    const auto& texCoordBufferView = model.bufferViews[texCoordAccessor.bufferView];
                    const float* texCoordData = reinterpret_cast<const float*>(&model.buffers[texCoordBufferView.buffer].data[texCoordBufferView.byteOffset + texCoordAccessor.byteOffset]);
                    for (size_t i = 0; i < texCoordAccessor.count; ++i)
                    {
                        mesh.uvs.emplace_back(glm::vec2(texCoordData[i * 2 + 0], texCoordData[i * 2 + 1]));
                    }
                }
                else {
                    assert(("Mesh has no TexCoords", false));
                }

                // if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
                //     const tinygltf::Accessor tangentAccessor = model.accessors[primitive.attributes.at("TANGENT")];
                //     const tinygltf::BufferView tangentBV = model.bufferViews[tangentAccessor.bufferView];
                //     const float* tangents = reinterpret_cast<const float*>(&model.buffers[tangentBV.buffer].data[tangentBV.byteOffset + tangentAccessor.byteOffset]);
                //     const glm::vec3& normal = mesh.normals.back();
                //     for (size_t i = 0; i < tangentAccessor.count; ++i)
                //     {
                //         mesh.tangents.emplace_back(glm::vec3(tangents[i * 3 + 0], tangents[i * 3 + 1], tangents[i * 3 + 2]));
                //         const glm::vec3& tangent = mesh.tangents.back();
                //         mesh.bitangents.emplace_back(glm::cross(tangent, normal));
                //     }
                // }
                // else {
                //     // calculate tangents as they are not available
                //     calculateTangents = true;
                // }

                // Get index data
                if (primitive.indices >= 0) {
                    const auto& indexAccessor = model.accessors[primitive.indices];
                    const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                    if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        const unsigned int* indices = reinterpret_cast<const unsigned int*>(&model.buffers[indexBufferView.buffer].data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                        for (size_t i = 0; i < indexAccessor.count; ++i)
                        {
                            mesh.indices.emplace_back(indices[i]);
                        }
                    }
                    else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        const unsigned short* indices = reinterpret_cast<const unsigned short*>(&model.buffers[indexBufferView.buffer].data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                        // mesh.indices.assign(indices, indices + indexAccessor.count);
                        assert("CANT HAVE SHORT INDICES");
                    }
                }
                else {
                    assert(("Mesh has no Normals", true));
                }


                if (calculateTangents)
                {
                    for (u32 i = 0; i < mesh.indices.size() - 3; i += 3)
                    {
                        u32 i0 = mesh.indices[i];
                        u32 i1 = mesh.indices[i + 1];
                        u32 i3 = mesh.indices[i + 2];
                    }
                }



                s32 materialIndex = primitive.material;
                if (materialIndex >= 0 && materialIndex < model.materials.size())
                {
                    const tinygltf::Material glMaterial = model.materials[materialIndex];
                    // Access base color texture
                    if (glMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                        s32 textureIndex = glMaterial.pbrMetallicRoughness.baseColorTexture.index;
                        const tinygltf::Texture& texture = model.textures[textureIndex];
                        // Access the corresponding image
                        if (texture.source >= 0 && texture.source < model.images.size()) {
                            const tinygltf::Image& image = model.images[texture.source];
                            std::shared_ptr<Image> myImage = std::make_shared<Image>(Image{ (u32)image.width, (u32)image.height, texture_format::RGBA, image.image });
                            images.emplace(textureIndex, myImage);
                            mesh.albedo = textureIndex;
                        }
                    }

                    // Access metallic roughness texture
                    if (glMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
                        s32 textureIndex = glMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
                        const auto& texture = model.textures[textureIndex];

                        // Access the corresponding image
                        if (texture.source >= 0 && texture.source < model.images.size()) {
                            const auto& image = model.images[texture.source];
                            std::shared_ptr<Image> myImage = std::make_shared<Image>(Image{ (u32)image.width, (u32)image.height, texture_format::RGBA, image.image });
                            images.emplace(textureIndex, myImage);
                            mesh.metallicRoughness = textureIndex;
                        }
                    }

                    // Access normal texture if it exists
                    if (glMaterial.normalTexture.index >= 0) {
                        s32 textureIndex = glMaterial.normalTexture.index;
                        const auto& texture = model.textures[textureIndex];

                        // Access the corresponding image
                        if (texture.source >= 0 && texture.source < model.images.size()) {
                            const auto& image = model.images[texture.source];
                            std::shared_ptr<Image> myImage = std::make_shared<Image>(Image{ (u32)image.width, (u32)image.height, texture_format::RGBA, image.image });
                            images.emplace(textureIndex, myImage);
                            mesh.normalMap = textureIndex;
                        }
                    }
                }
                result.meshes.push_back(mesh);
            }
        }
    }
    return result;
}
void application::BatchModel(Model& model)
{
    u32 vertexOffset = 0;
    u32 lastVertexCount = 0;
    u32 lastIndexCount = 0;
    for (const Mesh& mesh : model.meshes)
    {
        meshRenderData.push_back(RenderData{});
        RenderData& renderData = meshRenderData.back();
        renderData.meshOffsets.push_back(MeshOffset{ (u32)renderData.vertices.size(), (u32)renderData.indices.size(), (u32)mesh.indices.size()});
        for (size_t i = 0; i < mesh.positions.size(); ++i)
        {
            renderData.vertices.push_back(mesh.positions[i].x);
            renderData.vertices.push_back(mesh.positions[i].y);
            renderData.vertices.push_back(mesh.positions[i].z);
            renderData.vertices.push_back(mesh.normals[i].x);
            renderData.vertices.push_back(mesh.normals[i].y);
            renderData.vertices.push_back(mesh.normals[i].z);
            renderData.vertices.push_back(mesh.uvs[i].x);
            renderData.vertices.push_back(mesh.uvs[i].y);
        }

        // Add indices with offset
        for (const u32& index : mesh.indices) {
            renderData.indices.push_back(index + vertexOffset/8);
        }


        if (gpu_textures.find(mesh.albedo) == gpu_textures.end())
        {
            std::shared_ptr<Image>& image = images[mesh.albedo];

            GLuint textureID;

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
            glBindTexture(GL_TEXTURE_2D, 0);
        
            gpu_textures[mesh.albedo] = textureID;
            renderData.albedoID = textureID;
        }
        else {
            renderData.albedoID = gpu_textures[mesh.albedo];
        }

        if (gpu_textures.find(mesh.normalMap) == gpu_textures.end())
        {
            std::shared_ptr<Image>& image = images[mesh.normalMap];

            GLuint textureID;

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            gpu_textures[mesh.normalMap] = textureID;
            renderData.normalMapID = textureID;
        }
        else {
            renderData.normalMapID = gpu_textures[mesh.normalMap];
        }

        // Record mesh offset
        //vertexOffset += lastrenderData.vertices.size();
        //lastVertexCount += renderData.vertices.size();
        //lastIndexCount += renderData.indices.size();
    }
}
void application::UploadDataToGL(RenderData& data)
{
    glGenBuffers(1, &data.VBO);
    glGenBuffers(1, &data.EBO);
    glGenVertexArrays(1, &data.VAO);

    // Bind VAO
    glBindVertexArray(data.VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
    glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(f32), &data.vertices[0], GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), &data.indices[0], GL_STATIC_DRAW);

    // Define vertex attribute pointers (assuming layout: position, normal, UV)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);       // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));  // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));  // UV
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);
}
void application::renderQuad()
{
    if (quadVAO == 0)
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
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
float application::ourLerp(float a, float b, float f)
{
    return a + f * (b - a);
}
}

//glm::vec3 minBound(std::numeric_limits<float>::max(),
//    std::numeric_limits<float>::max(),
//    std::numeric_limits<float>::max());
//glm::vec3 maxBound(std::numeric_limits<float>::lowest(),
//    std::numeric_limits<float>::lowest(),
//    std::numeric_limits<float>::lowest());
//for (const Mesh& mesh : sponza.meshes)
//{
//    for (const auto& vertex : mesh.positions) {
//        minBound.x = std::min(minBound.x, vertex.x);
//        minBound.y = std::min(minBound.y, vertex.y);
//        minBound.z = std::min(minBound.z, vertex.z);
//
//        maxBound.x = std::max(maxBound.x, vertex.x);
//        maxBound.y = std::max(maxBound.y, vertex.y);
//        maxBound.z = std::max(maxBound.z, vertex.z);
//    }
//}
//minBound = minBound * 0.001f;
//maxBound = maxBound * 0.001f;
//glm::vec3 center = (minBound + maxBound) * 0.5f;
//glm::vec3 size = maxBound - minBound;
//
//f32 distance = std::max({ size.x, size.y, size.z }) * 1.0f;
