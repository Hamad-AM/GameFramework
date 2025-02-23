#include "common.h"

#include "application.h"
#include "input.h"
#include "event.h"
#include "components.h"
#include "render.h"
#include "asset_system.h"

#include <iostream>
#include <map>
#include <limits>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <random>

application::application() {}

application::~application()
{
    delete window;
}

void application::initialize(u32 screen_width, u32 screen_height)
{
    size_t perm_size = (size_t)4 * 1024 * 1024 * 1024;
    size_t trans_size = (size_t)256 * 1024 * 1024;

    permanent_storage.size = perm_size;
    permanent_storage.used = 0;
    permanent_storage.base = (u8*)VirtualAlloc(nullptr, perm_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    transient_storage.size = trans_size;
    transient_storage.used = 0;
    transient_storage.base = (u8*)VirtualAlloc(nullptr, trans_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    window = new glfw_window();
    screen_width_ = screen_width;
    screen_height_ = screen_height;
    window->initialize(screen_width_, screen_height_, "Game");

    camera2d_position = vec3(0.0f, 0.0f, 0.0f);
    camera2d.set_projection(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f));


    // sponza = load_model("assets/Sponza/sponza.glb");
    // sponza = load_model("assets/models/sphere.glb");
    

    CompileShaders(&renderData);

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

    GameState* state = PushStruct(&permanent_storage, GameState);

    vec3 sunPosition = { -46, 10.0, 20.0f };
    // state->lights[0] = {
    //         .type = Directional,
    //         .position = sunPosition,
    //         .direction = sunPosition - vec3(0),
    //         .color = { 1.0, 0.7, 0.39 },
    //         .luminance = 100000,
    //         .isShadowCasting = true,
    // };
    state->lights[0] = {
            .type = LightType::Directional,
            .position = sunPosition,
            .direction = sunPosition - vec3(0),
            .color = { 1.0, 0.7, 0.39 },
            .luminance = 200,
            .isShadowCasting = 1,
    };
    state->lights[1] = {
            .type = LightType::Point,
            .position = {3.6, 2.5, -2.9},
            .color = {1.0, 0.773, 0.561},
            .luminance = 50,
            .constant = 1.0, .linear=0.09, .quadratic=0.032,
            .isShadowCasting = 1,
    };

    state->numberOfLights = 2;

    lightPosition = glm::vec3();

    renderData.gpu_textures = gpu_textures;

    RenderSetupParameters(&renderData, screen_width_, screen_height_);

    size_t asset_size = 1024 * 1024 * 1024 * (u64)2;
    MemoryArena assetArena = InitArena(PushSize(&permanent_storage, asset_size), asset_size);
    InitAssetSystem(assets, assetArena);

    LoadScene(assets, "assets/models/bristro/bristro_interior.gltf", batchMeshRenderData, gpu_textures);

    camera = {};
    camera.position = glm::vec3(0.1f, 0.1f, 0.1f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.projection = glm::perspective(glm::radians(90.0f), (f32)screen_width_ / screen_height_, 0.01f, 1000.0f);
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
    SetupShadowMapPass(&renderData, state->lights, state->numberOfLights, shadow_width, shadow_height);
    SetupPointShadowMapPass(&renderData, state->lights, state->numberOfLights, shadow_width, shadow_height);

    SetupSSAOPass(&renderData);

    SetupGBuffers(&renderData);

    
    // SetupBRDFLUT(&renderData);


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


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f));

        camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

        update(dt);

        // RenderBRDFLUT(&renderData);

        RenderShadowMapPass(&renderData, batchMeshRenderData, model);
        RenderOmidirectionalShadowMap(&renderData, batchMeshRenderData, model, state->lights, state->numberOfLights);
        RenderDepthNormalPass(&renderData, batchMeshRenderData, model, camera);
        RenderSSAOPass(&renderData, camera);
        //
        // //RenderEnvironmentMap(&renderData, camera);
        // RenderScene(&renderData, batchMeshRenderData, model, camera, lightPosition);
        RenderDeferredScene(&renderData, batchMeshRenderData, model, camera);

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
        lightPosition += 1.0f;
    else if (input::is_key_down(key::DOWN))
        lightPosition.x -= 1.0f;
    else if (input::is_key_down(key::RIGHT))
        lightPosition.z += 1.0f;
    else if (input::is_key_down(key::LEFT))
        lightPosition.z -= 1.0f;
    else if (input::is_key_down(key::p))
        return;
    else if (input::is_key_down(key::r))
        CompileShaders(&renderData);

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
        MouseEvent press_event(event_type::left_mouse_button_press, input::mouse_position());
        event_system::get()->post_all(event_type::left_mouse_button_press, &press_event);
    }
    MouseEvent move_event(event_type::mouse_move, input::mouse_position());
    event_system::get()->post_all(event_type::mouse_move, &move_event);

    camera.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

}

void application::close()
{
    render.render_delete();
    window->close();
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
//f32 distance = std::max({ size.x, size.y, size.z }) * 1.0f;
