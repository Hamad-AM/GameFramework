#version 330 core

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec3 aNormal;    // Vertex normal (not used in unlit shading)
layout(location = 2) in vec2 aTexCoord;  // Texture coordinates

out vec2 TexCoord;  // Pass texture coordinates to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoord = aTexCoord;  // Pass texture coordinate to fragment shader
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
