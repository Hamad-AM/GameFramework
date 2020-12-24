#shader vertex

#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

out vec3 v_Color;
out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position, 1.0f);
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
}

#shader fragment

#version 330 core
out vec4 color;

in vec3 v_Color;
in vec2 v_TexCoord;

uniform sampled2D u_Texture;

void main()
{
    color = texture(u_Texture, v_TexCoord); //* v_Color;
}