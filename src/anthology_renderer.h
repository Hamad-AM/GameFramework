#ifndef ANTHOLOGY_RENDERER_H
#define ANTHOLOGY_RENDERER_H

#include <string>

#include <glad/glad.h>
#include <stb_image.h>

#include "anthology_math.h"
#include "anthology_types.h"

#include "anthology_opengl.h"

void draw_texture_cube(TextureCube* cube, PerspectiveCamera* camera, Environment* env);

void draw_cube(Cube* cube, PerspectiveCamera* camera);

void update_camera(PerspectiveCamera* camera);

void init_cube(Cube* cube);

void init_texture_cube(TextureCube* cube);

void free_loaded_texture(LoadedTexture2D* loaded_texture);

LoadedTexture2D load_texture(const char* file_path, TextureFilter filter, TextureWrap wrapping);
#endif