#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 tex_coord;

out vec2 TexCoords;

uniform mat4 u_model;
uniform mat4 u_projection_view;
uniform float u_tiling_factor;

void main()
{
    TexCoords = tex_coord * u_tiling_factor;
    gl_Position = u_projection_view * u_model * vec4(vertex, 1.0);
}
