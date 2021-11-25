// #pragma once

// #include "platform.h"

// #include "shader.h"
// #include "texture.h"
// #include "math.h"
// using namespace math;

// class RenderDevice
// {
// public:
//     RenderDevice();

//     void Draw(Shader& shader, u32& vertex_array);

//     void Clear(v3& color, b32 clear_depth_buffer);

//     Shader* CreateShader(const char* vs, const char* fs);
//     Texture2D* CreateTexture(const char* filename);

// private:
//     std::vector<Texture2D> textures;
//     std::vector<Shader> shaders;
// };