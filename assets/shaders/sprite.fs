#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D u_texture;
uniform vec3 spriteColor;

void main()
{
    color = texture(u_texture, TexCoords) * vec4(spriteColor, 1.0f);
}
