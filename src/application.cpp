#include "common.h"

#include "application.h"
#include "input.h"
#include "components.h"

#include <iostream>
#include <map>
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

    camera = {};
    camera.projection  = glm::perspective(glm::radians(90.0f), (f32)screen_width/screen_height, 0.1f, 100.0f);
    camera.position    = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.orientation = glm::quat(glm::vec3(0.f, 0.f, 0.f));

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

                // Get vertex data
                const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
                const float* positions = reinterpret_cast<const float*>(&model.buffers[positionBufferView.buffer].data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

                for (size_t i = 0; i < positionAccessor.count; ++i)
                {
                    mesh.positions.emplace_back(glm::vec3(positions[i*3 + 0], positions[i*3 + 1], positions[i*3 + 2]));
                }

                // Get normal data if available
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
                    const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
                    const float* normals = reinterpret_cast<const float*>(&model.buffers[normalBufferView.buffer].data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
                    for (size_t i = 0; i < normalAccessor.count; ++i)
                    {
                        mesh.normals.emplace_back(glm::vec3(normals[i*3 + 0], normals[i*3 + 1], normals[i*3 + 2]));
                    }
                }

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
                    } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        const unsigned short* indices = reinterpret_cast<const unsigned short*>(&model.buffers[indexBufferView.buffer].data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                        // mesh.indices.assign(indices, indices + indexAccessor.count);
                        assert("CANT HAVE SHORT INDICES");
                    }
                }

                if (primitive.attributes.count("TEXCOORD_0")) {
                    const auto& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                    const auto& texCoordBufferView = model.bufferViews[texCoordAccessor.bufferView];
                    const float* texCoordData = reinterpret_cast<const float*>(&model.buffers[texCoordBufferView.buffer].data[texCoordBufferView.byteOffset + texCoordAccessor.byteOffset]);
                    for (size_t i = 0; i < texCoordAccessor.count; ++i)
                    {
                        mesh.uvs.emplace_back(glm::vec2(texCoordData[i*2 + 0], texCoordData[i*2 + 1]));
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
                            std::shared_ptr<Image> myImage = std::make_shared<Image>(image.width, image.height, texture_format::RGBA, image.image);
                            images.emplace(textureIndex, myImage);
                            mesh.alebdo = images.at(textureIndex);
                        }
                    }

                    // Access metallic roughness texture
                    if (glMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
                        s32 textureIndex = glMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
                        const auto& texture = model.textures[textureIndex];

                        // Access the corresponding image
                        if (texture.source >= 0 && texture.source < model.images.size()) {
                            const auto& image = model.images[texture.source];
                            std::shared_ptr<Image> myImage = std::make_shared<Image>(image.width, image.height, texture_format::RGBA, image.image);
                            images.emplace(textureIndex, myImage);
                            mesh.metallicRoughness = images[textureIndex];
                        }
                    }

                    // Access normal texture if it exists
                    if (glMaterial.normalTexture.index >= 0) {
                        s32 textureIndex = glMaterial.normalTexture.index;
                        const auto& texture = model.textures[textureIndex];

                        // Access the corresponding image
                        if (texture.source >= 0 && texture.source < model.images.size()) {
                            const auto& image = model.images[texture.source];
                            std::shared_ptr<Image> myImage = std::make_shared<Image>(image.width, image.height, texture_format::RGBA, image.image);
                            images.emplace(textureIndex, myImage);
                            mesh.normalMap = images[textureIndex];
                        }
                    }
                }
                sponza.meshes.push_back(mesh);
            }
        }

    }

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

    while (!window->should_close())
    {
        window->handle_input();
        // state.audio.update();

        f64 current_time = window->get_time();
        f64 dt = current_time - previous_time;
        previous_time = current_time;

        if (dt < 0)
            dt = 0.0001;
        
        update(dt);

        window->swap_buffers();
    }
}

void application::update(f32 dt)
{
    if (input::is_key_down(key::d))
        camera2d_position.x += 0.01f;
    else if (input::is_key_down(key::a))
        camera2d_position.x -= 0.01f;
    else if (input::is_key_down(key::w))
        camera2d_position.y += 0.01f;
    else if (input::is_key_down(key::s))
        camera2d_position.y -= 0.01f;
    else if (input::is_key_down(key::p))
        return;

    if (input::is_left_mouse_down())
    {
        mouse_event press_event{event_type::left_mouse_button_press, input::mouse_position()};
        event_system::get()->post_all(event_type::left_mouse_button_press, &press_event);
    }
    mouse_event move_event{event_type::mouse_move, input::mouse_position()};
    event_system::get()->post_all(event_type::mouse_move, &move_event);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
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
}
