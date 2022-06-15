#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D u_texture;
uniform vec4 spriteColor;

void main()
{
    color = spriteColor * texture(u_texture, TexCoords);
}