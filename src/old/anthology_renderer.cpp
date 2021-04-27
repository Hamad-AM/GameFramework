#include "anthology_renderer.h"

#include <vector>
#include <fstream>
#include <string>



LoadedTexture2D load_texture(const char* file_path, TextureFilter filter, TextureWrap wrapping)
{
    stbi_set_flip_vertically_on_load(1);

    LoadedTexture2D loaded_texture = {};
    loaded_texture.filter = filter;
    loaded_texture.wrap = wrapping;
    loaded_texture.image_data = stbi_load(file_path, &loaded_texture.width, &loaded_texture.height, &loaded_texture.number_of_channels, 0);
    loaded_texture.path = file_path;

    assert(loaded_texture.image_data);

    return loaded_texture;
}

void free_loaded_texture(LoadedTexture2D* loaded_texture)
{
    stbi_image_free(loaded_texture->image_data);
    *loaded_texture = {};
}


// void init_texture_cube(TextureCube* cube)
// {
//     float vertices[] = {
//         -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
//         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,1.0f, 0.0f,
//         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,1.0f, 1.0f,
//         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,1.0f, 1.0f,
//         -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,0.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,0.0f, 0.0f,

//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//         0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,1.0f, 0.0f,
//         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,1.0f, 1.0f,
//         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,1.0f, 1.0f,
//         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,0.0f, 1.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,0.0f, 0.0f,

//         -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
//         -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,1.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,0.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,0.0f, 1.0f,
//         -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,0.0f, 0.0f,
//         -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,1.0f, 0.0f,

//         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
//         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
//         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
//         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
//         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
//         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

//         -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
//         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
//         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//         -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
//         -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

//         -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
//         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,1.0f, 1.0f,
//         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,1.0f, 0.0f,
//         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,1.0f, 0.0f,
//         -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,0.0f, 0.0f,
//         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f
//     };

//     memcpy(cube->vertices, vertices, sizeof(vertices));

//     uint32 indices[6] = {
//         0, 1, 3,
//         1, 2, 3 
//     };

//     BufferLayoutAttrib layout[3] = {
//         {ShaderType::SHADER_FLOAT, 3, false},
//         {ShaderType::SHADER_FLOAT, 3, false},
//         {ShaderType::SHADER_FLOAT, 2, false},
//     };

//     create_vertex_array(&cube->va);
//     create_vertex_buffer(&cube->vb, vertices, sizeof(vertices));

//     assign_vertex_attrib_layout(layout, 3);   
//     load_shader(&cube->shader, "..\\data\\shaders\\Texture.glsl");

//     unbind_vertex_array(&cube->va);
//     unbind_vertex_buffer();
//     unbind_index_buffer();

// }

void update_camera(PerspectiveCamera* camera)
{
    camera->view = glm::translate(glm::mat4(1.0f), camera->position);
    camera->projection_view = camera->projection * camera->view;
}

// void draw_texture_cube(TextureCube* cube, PerspectiveCamera* camera, Environment* env)
// {
//     bind_texture(&cube->material.diffuse);
//     bind_texture(&cube->material.specular);
//     bind_vertex_array(&cube->va);
//     bind_shader(&cube->shader);
    
//     glm::mat4 model = glm::mat4(1.0f);
//     model = glm::translate(model, cube->position);

//     upload_uniform_mat4("u_ProjectionViewMat", camera->projection_view, &cube->shader);
//     upload_uniform_mat4("u_ProjectionViewMat", camera->projection_view, &cube->shader);
//     upload_uniform_mat4("u_ModelMat", model, &cube->shader);
//     upload_uniform_vec3("u_ViewPosition", camera->position, &cube->shader);

//     upload_material_uniform(&cube->material, &cube->shader);

//     // upload_directional_light_uniform(&env->dir_light, &cube->shader);
//     // upload_spot_light_uniform(&env->spot_light, &cube->shader);
//     for (int i = 0; i < NO_POINT_LIGHTS; ++i)
//        upload_point_light_uniform(&env->point_lights[i], i, &cube->shader);
    
//     glDrawArrays(GL_TRIANGLES, 0, 36);
    
//     unbind_shader(&cube->shader);
//     unbind_vertex_array(&cube->va);
//     unbind_texture(&cube->material.diffuse);
// }