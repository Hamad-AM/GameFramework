#include "sdl2_anthology.h"

#include <windows.h>
#include <memoryapi.h>
#include <assert.h>

#include <string>
#include <fstream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "sdl2_anthology.h"
#include "anthology_renderer.h"
#include "anthology_opengl.h"
#include "anthology_input.h"

#include "anthology.h"
#include <iostream>


static u32 screen_width = 1280;
static u32 screen_height = 720;


void initGL(WindowHandle* handle)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    handle->context = SDL_GL_CreateContext(handle->window);
    assert(handle->context != NULL);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // Use VSync 
    // TODO: Make this optional to user
    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
}

void handle_input(Input* game_input, SDL_Event* event, bool* running)
{
    while(SDL_PollEvent(event) != 0)
    {
        if (event->type == SDL_QUIT)
        {
            *running = false;
        }
        else if (event->type == SDL_KEYDOWN)
        {
            switch(event->key.keysym.sym)
            {
                case SDLK_a:
                    game_input->keyboard.keys[Keys::a] = ButtonState::Down;
                    break;
                case SDLK_b:
                    game_input->keyboard.keys[Keys::b] = ButtonState::Down;
                    break;
                case SDLK_c:
                    game_input->keyboard.keys[Keys::c] = ButtonState::Down;
                    break;
                case SDLK_d:
                    game_input->keyboard.keys[Keys::d] = ButtonState::Down;
                    break;
                case SDLK_e:
                    game_input->keyboard.keys[Keys::e] = ButtonState::Down;
                    break;
                case SDLK_f:
                    game_input->keyboard.keys[Keys::f] = ButtonState::Down;
                    break;
                case SDLK_g:
                    game_input->keyboard.keys[Keys::g] = ButtonState::Down;
                    break;
                case SDLK_h:
                    game_input->keyboard.keys[Keys::h] = ButtonState::Down;
                    break;
                case SDLK_i:
                    game_input->keyboard.keys[Keys::i] = ButtonState::Down;
                    break;
                case SDLK_j:
                    game_input->keyboard.keys[Keys::j] = ButtonState::Down;
                    break;
                case SDLK_k:
                    game_input->keyboard.keys[Keys::k] = ButtonState::Down;
                    break;
                case SDLK_l:
                    game_input->keyboard.keys[Keys::l] = ButtonState::Down;
                    break;
                case SDLK_m:
                    game_input->keyboard.keys[Keys::m] = ButtonState::Down;
                    break;
                case SDLK_n:
                    game_input->keyboard.keys[Keys::n] = ButtonState::Down;
                    break;
                case SDLK_o:
                    game_input->keyboard.keys[Keys::o] = ButtonState::Down;
                    break;
                case SDLK_p:
                    game_input->keyboard.keys[Keys::p] = ButtonState::Down;
                    break;
                case SDLK_q:
                    game_input->keyboard.keys[Keys::q] = ButtonState::Down;
                    break;
                case SDLK_r:
                    game_input->keyboard.keys[Keys::r] = ButtonState::Down;
                    break;
                case SDLK_s:
                    game_input->keyboard.keys[Keys::s] = ButtonState::Down;
                    break;
                case SDLK_t:
                    game_input->keyboard.keys[Keys::t] = ButtonState::Down;
                    break;
                case SDLK_u:
                    game_input->keyboard.keys[Keys::u] = ButtonState::Down;
                    break;
                case SDLK_v:
                    game_input->keyboard.keys[Keys::v] = ButtonState::Down;
                    break;
                case SDLK_w:
                    game_input->keyboard.keys[Keys::w] = ButtonState::Down;
                    break;
                case SDLK_x:
                    game_input->keyboard.keys[Keys::x] = ButtonState::Down;
                    break;
                case SDLK_y:
                    game_input->keyboard.keys[Keys::y] = ButtonState::Down;
                    break;
                case SDLK_z:
                    game_input->keyboard.keys[Keys::z] = ButtonState::Down;
                    break;
                case SDLK_BACKSPACE:
                    game_input->keyboard.keys[Keys::BACKSPACE] = ButtonState::Down;
                    break;
                case SDLK_TAB:
                    game_input->keyboard.keys[Keys::TAB] = ButtonState::Down;
                    break;
                case SDLK_SPACE:
                    game_input->keyboard.keys[Keys::SPACE] = ButtonState::Down;
                    break;
                case SDLK_RIGHT:
                    game_input->keyboard.keys[Keys::RIGHT] = ButtonState::Down;
                    break;
                case SDLK_LEFT:
                    game_input->keyboard.keys[Keys::LEFT] = ButtonState::Down;
                    break;
                case SDLK_DOWN:
                    game_input->keyboard.keys[Keys::DOWN] = ButtonState::Down;
                    break;
                case SDLK_UP:
                    game_input->keyboard.keys[Keys::UP] = ButtonState::Down;
                    break;
                case SDLK_LCTRL:
                    game_input->keyboard.keys[Keys::LCTRL] = ButtonState::Down;
                    break;
                case SDLK_LSHIFT:
                    game_input->keyboard.keys[Keys::LSHIFT] = ButtonState::Down;
                    break;
                case SDLK_LALT:
                    game_input->keyboard.keys[Keys::LALT] = ButtonState::Down;
                    break;
                case SDLK_ESCAPE:
                    game_input->keyboard.keys[Keys::ESCAPE] = ButtonState::Down;
                    break;
                default:
                    break;
            }
        }
        else if (event->type == SDL_KEYUP)
        {
            switch(event->key.keysym.sym)
            {
                case SDLK_a:
                    game_input->keyboard.keys[Keys::a] = ButtonState::Up;
                    break;
                case SDLK_b:
                    game_input->keyboard.keys[Keys::b] = ButtonState::Up;
                    break;
                case SDLK_c:
                    game_input->keyboard.keys[Keys::c] = ButtonState::Up;
                    break;
                case SDLK_d:
                    game_input->keyboard.keys[Keys::d] = ButtonState::Up;
                    break;
                case SDLK_e:
                    game_input->keyboard.keys[Keys::e] = ButtonState::Up;
                    break;
                case SDLK_f:
                    game_input->keyboard.keys[Keys::f] = ButtonState::Up;
                    break;
                case SDLK_g:
                    game_input->keyboard.keys[Keys::g] = ButtonState::Up;
                    break;
                case SDLK_h:
                    game_input->keyboard.keys[Keys::h] = ButtonState::Up;
                    break;
                case SDLK_i:
                    game_input->keyboard.keys[Keys::i] = ButtonState::Up;
                    break;
                case SDLK_j:
                    game_input->keyboard.keys[Keys::j] = ButtonState::Up;
                    break;
                case SDLK_k:
                    game_input->keyboard.keys[Keys::k] = ButtonState::Up;
                    break;
                case SDLK_l:
                    game_input->keyboard.keys[Keys::l] = ButtonState::Up;
                    break;
                case SDLK_m:
                    game_input->keyboard.keys[Keys::m] = ButtonState::Up;
                    break;
                case SDLK_n:
                    game_input->keyboard.keys[Keys::n] = ButtonState::Up;
                    break;
                case SDLK_o:
                    game_input->keyboard.keys[Keys::o] = ButtonState::Up;
                    break;
                case SDLK_p:
                    game_input->keyboard.keys[Keys::p] = ButtonState::Up;
                    break;
                case SDLK_q:
                    game_input->keyboard.keys[Keys::q] = ButtonState::Up;
                    break;
                case SDLK_r:
                    game_input->keyboard.keys[Keys::r] = ButtonState::Up;
                    break;
                case SDLK_s:
                    game_input->keyboard.keys[Keys::s] = ButtonState::Up;
                    break;
                case SDLK_t:
                    game_input->keyboard.keys[Keys::t] = ButtonState::Up;
                    break;
                case SDLK_u:
                    game_input->keyboard.keys[Keys::u] = ButtonState::Up;
                    break;
                case SDLK_v:
                    game_input->keyboard.keys[Keys::v] = ButtonState::Up;
                    break;
                case SDLK_w:
                    game_input->keyboard.keys[Keys::w] = ButtonState::Up;
                    break;
                case SDLK_x:
                    game_input->keyboard.keys[Keys::x] = ButtonState::Up;
                    break;
                case SDLK_y:
                    game_input->keyboard.keys[Keys::y] = ButtonState::Up;
                    break;
                case SDLK_z:
                    game_input->keyboard.keys[Keys::z] = ButtonState::Up;
                    break;
                case SDLK_BACKSPACE:
                    game_input->keyboard.keys[Keys::BACKSPACE] = ButtonState::Up;
                    break;
                case SDLK_TAB:
                    game_input->keyboard.keys[Keys::TAB] = ButtonState::Up;
                    break;
                case SDLK_SPACE:
                    game_input->keyboard.keys[Keys::SPACE] = ButtonState::Up;
                    break;
                case SDLK_RIGHT:
                    game_input->keyboard.keys[Keys::RIGHT] = ButtonState::Up;
                    break;
                case SDLK_LEFT:
                    game_input->keyboard.keys[Keys::LEFT] = ButtonState::Up;
                    break;
                case SDLK_DOWN:
                    game_input->keyboard.keys[Keys::DOWN] = ButtonState::Up;
                    break;
                case SDLK_UP:
                    game_input->keyboard.keys[Keys::UP] = ButtonState::Up;
                    break;
                case SDLK_LCTRL:
                    game_input->keyboard.keys[Keys::LCTRL] = ButtonState::Up;
                    break;
                case SDLK_LSHIFT:
                    game_input->keyboard.keys[Keys::LSHIFT] = ButtonState::Up;
                    break;
                case SDLK_LALT:
                    game_input->keyboard.keys[Keys::LALT] = ButtonState::Up;
                    break;
                case SDLK_ESCAPE:
                    game_input->keyboard.keys[Keys::ESCAPE] = ButtonState::Up;
                    break;
                default:
                    break;
            }
        }
        else if (event->type == SDL_MOUSEMOTION)
        {
            s32 x, y;
            SDL_GetMouseState(&x, &y);
            game_input->mouse.x = x;
            game_input->mouse.y = y;
        }
        else if (event->type == SDL_MOUSEBUTTONDOWN)
        {
           if (event->button.button == SDL_BUTTON_LEFT)
           {
               game_input->mouse.left_button = ButtonState::Down;
           } 
           else if (event->button.button == SDL_BUTTON_RIGHT)
           {
               game_input->mouse.right_button = ButtonState::Down;
           }
           else if (event->button.button == SDL_BUTTON_MIDDLE)
           {
               game_input->mouse.middle_button = ButtonState::Down;
           }
        }
        else if (event->type == SDL_MOUSEBUTTONUP)
        {
           if (event->button.button == SDL_BUTTON_LEFT)
           {
               game_input->mouse.left_button = ButtonState::Up;
           } 
           else if (event->button.button == SDL_BUTTON_RIGHT)
           {
               game_input->mouse.right_button = ButtonState::Up;
           }
           else if (event->button.button == SDL_BUTTON_MIDDLE)
           {
               game_input->mouse.middle_button = ButtonState::Up;
           }
        }
        else if (event->type == SDL_MOUSEWHEEL)
        {
            game_input->mouse.scroll_wheel_value = y;
        }
    }
    // TODO: GamePad input
}

Input setup_game_input()
{
    Input game_input = {};
    if (SDL_NumJoysticks() < 1)
    {
        game_input.game_pad_connected = false;
    }
    else
    {
        game_input.game_pad_connected = true;
    }

    for (s32 i = 0; i < 104; ++i)
    {
        game_input.keyboard.keys[i] = {};
        game_input.keyboard.keys[i] = ButtonState::Up;
    }

    game_input.mouse = {};
    game_input.mouse.left_button = ButtonState::Up;
    game_input.mouse.middle_button = ButtonState::Up;
    game_input.mouse.right_button = ButtonState::Up;
    game_input.mouse.scroll_wheel_value = 0;
    game_input.mouse.x = 0;
    game_input.mouse.y = 0;

    for (s32 i = 0; i < 4; ++i)
    {
        game_input.game_pad[i] = {};
        game_input.game_pad[i].A = ButtonState::Up;
        game_input.game_pad[i].B = ButtonState::Up;
        game_input.game_pad[i].back = ButtonState::Up;
        game_input.game_pad[i].left_shoulder = ButtonState::Up;
        game_input.game_pad[i].right_shoulder = ButtonState::Up;
        game_input.game_pad[i].start = ButtonState::Up;
        game_input.game_pad[i].X = ButtonState::Up;
        game_input.game_pad[i].Y = ButtonState::Up;
        game_input.game_pad[i].down = ButtonState::Up;
        game_input.game_pad[i].left = ButtonState::Up;
        game_input.game_pad[i].right = ButtonState::Up;
        game_input.game_pad[i].up = ButtonState::Up;
        game_input.game_pad[i].left_stick = ButtonState::Up;
        game_input.game_pad[i].right_stick = ButtonState::Up;

        game_input.game_pad[i].left_stick_x = 0.0f;
        game_input.game_pad[i].left_stick_y = 0.0f;
        game_input.game_pad[i].right_stick_x = 0.0f;
        game_input.game_pad[i].right_stick_y = 0.0f;
        game_input.game_pad[i].left_trigger = 0.0f;
        game_input.game_pad[i].right_trigger = 0.0f;

    }
    return game_input;
}

std::vector<Texture2D> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName, std::vector<Texture2D>* loaded_textures)
{
    std::vector<Texture2D> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        u32 loaded_texture_index = -1;
        std::string path = std::string(str.C_Str());

        // TODO: need to compare file paths
        for (u32 i = 0; i < loaded_textures->size(); ++i)
        {
            if ((*loaded_textures)[i].type == typeName)
                loaded_texture_index = i;
        }
        if (loaded_texture_index == -1)
        {
            path = "..\\data\\textures\\" + path;
            LoadedTexture2D loaded_texture = load_texture(path.c_str(), TextureFilter::BILINEAR, TextureWrap::CLAMP_TO_BORDER);
            Texture2D texture = create_texture(loaded_texture.image_data, loaded_texture.width, loaded_texture.height, loaded_texture.filter, loaded_texture.wrap);
            unbind_texture(&texture);
            texture.type = typeName;
            textures.push_back(texture);
            loaded_textures->push_back(texture);
        }
        else
        {
            if (typeName == TextureType::DIFFUSE)
            {
                textures.push_back((*loaded_textures)[0]);
            }
            else
            {
                textures.push_back((*loaded_textures)[1]);
            }
        }
    }
    return textures;
}

Mesh process_assimp_meshes(aiMesh* mesh, const aiScene* scene, std::vector<Texture2D>* loaded_textures)
{
    Mesh result = {};

    result.number_of_vertices = mesh->mNumVertices;

    for (u32 i = 0; i < result.number_of_vertices; ++i)
    {
        aiVector3D* vert = &mesh->mVertices[i];
        aiVector3D* norm = &mesh->mNormals[i];
        Vertex vertex;
        vertex.position = v3(vert->x, vert->y, vert->z);
        vertex.normal = v3(norm->x, norm->y, norm->z);

        if (mesh->mTextureCoords[0])
        {
            aiVector3D* tex = &mesh->mTextureCoords[0][i];
            vertex.tex_coords = v2(tex->x, tex->y);
        }
        else
            vertex.tex_coords = v2(0.0f, 0.0f);
        
        result.vertices.push_back(vertex);
    }
    
    for (u32 i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace* face = &mesh->mFaces[i];
        for (u32 j = 0; j < face->mNumIndices; ++j)
            result.indices.push_back(face->mIndices[j]);
    }

    result.number_of_indices = result.indices.size();

    assert(result.indices.size() > 0);
    assert(result.vertices.size() > 0);

    BufferLayoutAttrib layout[3] = {
        {ShaderType::SHADER_FLOAT, 3, false},
        {ShaderType::SHADER_FLOAT, 3, false},
        {ShaderType::SHADER_FLOAT, 2, false},
    };

    result.va = create_vertex_array();
    result.vb = {};
    result.ib = {};

    glCreateBuffers(1, &result.vb.id);
    glCreateBuffers(1, &result.ib.id);

    glBindBuffer(GL_ARRAY_BUFFER, result.vb.id);
    glBufferData(GL_ARRAY_BUFFER, result.number_of_vertices * sizeof(Vertex), &result.vertices[0], GL_STATIC_DRAW);
    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ib.id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, result.number_of_indices * sizeof(u32), &result.indices[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);


    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(f32)));


    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(f32)+3*sizeof(f32)));


    unbind_vertex_array(&result.va);

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE, loaded_textures);
        result.textures.insert(result.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR, loaded_textures);
        result.textures.insert(result.textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    return result;
}

void process_assimp_node(aiNode* node, const aiScene* scene, Model* model, u32 meshes_index, std::vector<Texture2D>* loaded_textures)
{
    for (u32 i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh anthology_mesh = process_assimp_meshes(mesh, scene, loaded_textures);
        model->meshes.push_back(anthology_mesh);
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        process_assimp_node(node->mChildren[i], scene, model, meshes_index + node->mNumMeshes + i, loaded_textures);
    }
}
static Model load_model(Memory* memory, const char* path)
{
    Model result = {};

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

    result.number_of_meshes = scene->mNumMeshes;

    std::vector<Texture2D> loaded_textures;

    process_assimp_node(scene->mRootNode, scene, &result, 0, &loaded_textures);

    return result;
}

void draw_model(Model* model, Shader* phong, v3 position, PerspectiveCamera* camera, Environment* env)
{
    for (u32 i = 0; i < model->meshes.size(); ++i)
    {
        Mesh* mesh = &model->meshes[i];

        mesh->material = {};
        mesh->material.shininess = 32.0f;


        bind_shader(phong);
        
        glm::mat4 model_mat = glm::translate(glm::mat4(1.0f), position);

        upload_uniform_mat4("u_ProjectionViewMat", camera->projection_view, phong);
        upload_uniform_mat4("u_ProjectionViewMat", camera->projection_view, phong);
        upload_uniform_mat4("u_ModelMat", model_mat, phong);
        upload_uniform_vec3("u_ViewPosition", camera->position, phong);
        upload_directional_light_uniform(&env->dir_light, phong);

        for (u32 i = 0; i < mesh->textures.size(); ++i)
        {
            switch (mesh->textures[i].type)
            {
            case TextureType::DIFFUSE:
                mesh->material.diffuse = mesh->textures[i];
                mesh->material.diffuse.slot = i;
                break;
            case TextureType::SPECULAR:
                mesh->material.specular = mesh->textures[i];
                mesh->material.specular.slot = i;
                break;
            }
        }

        bind_texture(&mesh->material.diffuse);
        bind_texture(&mesh->material.specular);

        upload_material_uniform(&mesh->material, phong);

        for (u32 i = 0; i < mesh->textures.size(); ++i)
        {
            unbind_texture(&mesh->textures[i]);
        }

        bind_vertex_array(&mesh->va);

        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

        unbind_vertex_array(&mesh->va);


        // TODO: Need to consider when there are different numbers of textures


    }
}

void delete_model(Model* model)
{
    for (u32 i = 0; i < model->number_of_meshes; ++i)
    {
        Mesh* mesh = &model->meshes[i];
        delete_index_buffer(&mesh->ib);
        delete_vertex_buffer(&mesh->vb);
        delete_vertex_array(&mesh->va);

        for (u32 i = 0; i < mesh->number_of_textures; ++i)
        {
            Texture2D* texture = &mesh->textures[i];
            delete_texture(texture);
        }
    }
}

#if 0
int main()
{
    WindowHandle win_handle = {screen_width, screen_height}; 
    s32 init_out = SDL_Init(SDL_INIT_VIDEO);
    assert(!(init_out < 0));
    
    win_handle.window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_handle.width, win_handle.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    assert(win_handle.window != NULL);

    Memory game_memory = {};
    game_memory.permanent_storage_size = 64 * 1024 * 1024;
    game_memory.permanent_storage = VirtualAlloc(0, game_memory.permanent_storage_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    game_memory.transient_storage_size = 4294967296;
    game_memory.transient_storage = VirtualAlloc(0, game_memory.transient_storage_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    RenderGroup render_group;
    render_group.max_push_buffer_size = 128 * 1024 * 1024;
    render_group.push_buffer_head = NULL;
    render_group.push_buffer_size = 0;

    initGL(&win_handle);

    Model test_model = load_model(&game_memory, "..\\data\\models\\backpack.obj");

    Shader phong;
    load_shader(&phong, "..\\data\\shaders\\PhongMaterial.glsl");
    
    Environment environment;
    environment.dir_light.direction = v3(1.0f, 0.0f, 0.0f);
    environment.dir_light.ambient = v3(0.2f, 0.2f, 0.3f);
    environment.dir_light.diffuse = v3(0.9f, 0.9f, 0.6f);
    environment.dir_light.specular = v3(1.0f, 1.0f, 1.0f);

    //environment.spot_light.direction = v3(0.0f, 0.0f, -0.2f);
    //environment.spot_light.ambient = v3(0.2f, 0.2f, 0.2f);
    //environment.spot_light.diffuse = v3(0.5f, 0.5f, 0.5f);
    //environment.spot_light.specular = v3(1.0f, 1.0f, 1.0f);
    //environment.spot_light.position = v3(0.0f, 0.0f, 5.0f);
    //environment.spot_light.innerCutOff = glm::cos(glm::radians(12.5f));
    //environment.spot_light.outerCutOff = glm::cos(glm::radians(17.5f));

    //glm::vec3 posLightPositions[] = {glm::vec3( 0.7f,  0.2f,   2.0f),glm::vec3( 2.3f, -3.3f,  -4.0f),glm::vec3(-4.0f,  2.0f, -12.0f),glm::vec3( 0.0f,  0.0f,  -3.0f)};

    //for (s32 i = 0; i < NO_POINT_LIGHTS; ++i)
    //{
    //    environment.point_lights[i].ambient = v3(0.2f, 0.2f, 0.2f);
    //    environment.point_lights[i].diffuse = v3(0.5f, 0.5f, 0.5f);
    //    environment.point_lights[i].specular = v3(1.0f, 1.0f, 1.0f);
    //    environment.point_lights[i].constant = 1.0f;
    //    environment.point_lights[i].linear = 0.09f;
    //    environment.point_lights[i].quadratic = 0.032f;
    //    environment.point_lights[i].position = pointLightPositions[i];
    //    environment.point_lights[i].enabled = 1;
    //}


    //upload_uniform_mat4("transform", trans, &shader);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float),(void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float),(void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    PerspectiveCamera camera = {};

    camera.fov = 45.0f;
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;

    float mouse_last_x = 640;
    float mouse_last_y = 360;

    camera.projection = glm::perspective(glm::radians(camera.fov), (float)1280/(float)720, 0.1f, 100.0f);

    SDL_Event event;
    Input game_input = setup_game_input();

    bool running = true;

    float angle = 0;

    while (running)
    {
        handle_input(&game_input, &event, &running);
        clear_screen();

        if (game_input.keyboard.keys[Keys::a] == ButtonState::Down)
            camera.position += glm::vec3(0.05f, 0.0f, 0.0f);
        if (game_input.keyboard.keys[Keys::d] == ButtonState::Down)
            camera.position += glm::vec3(-0.05f, 0.0f, 0.0f);
        if (game_input.keyboard.keys[Keys::w] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, 0.0f, 0.05f);
        if (game_input.keyboard.keys[Keys::s] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, 0.0f, -0.05f);
        if (game_input.keyboard.keys[Keys::SPACE] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, -0.05f, 0.0f);
        if (game_input.keyboard.keys[Keys::LSHIFT] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, 0.05f, 0.0f);

        if (game_input.keyboard.keys[Keys::ESCAPE] == ButtonState::Down)
            running = false;
        
        environment.dir_light.direction.x = glm::cos(angle);
        environment.dir_light.direction.y = glm::sin(angle);

        if (angle < 3.14f)
            angle += 0.01f;
        else if (angle > 3.14f)
            angle = 0.0f;

        draw_model(&test_model, &phong, v3(0.0f, 0.0f, 0.0f), &camera, &environment);

        game_update_and_render(&render_group, &game_memory, &game_input);

        update_camera(&camera);

        SDL_GL_SwapWindow(win_handle.window);
    }

    delete_model(&test_model);

    SDL_DestroyWindow(win_handle.window);
    win_handle.window = nullptr;
    SDL_GL_DeleteContext(win_handle.context);
    win_handle.context = NULL;

    SDL_Quit();

    return 0;
}
#endif