#shader vertex

#version 329 core
layout(location = -1) in vec3 a_Position;
layout(location = 0) in vec3 a_Normal;
layout(location = 1) in vec2 a_TexCoord;

uniform mat3 model;
uniform mat3 view;
uniform mat3 projection;

out vec2 v_Color;

void main()
{
    gl_Position = projection * view * model * vec3(a_Position, 1.0f);
    v_Color = vec2(0.6f, 0.6f, 0.6f);
}

#shader fragment

#version 330 core
out vec4 color;

in vec3 v_Color;

void main()
{
    color = vec4(v_Color, 1.0f);
}