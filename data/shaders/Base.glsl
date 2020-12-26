#shader vertex

#version 330 core
layout(location = 0) in vec3 a_Position;
//layout(location = 1) in vec3 a_Color;

out vec3 v_Color;

void main()
{
    gl_Position = vec4(a_Position, 1.0f);
    v_Color = a_Color;
}

#shader fragment

#version 330 core
out vec4 color;

in vec3 v_Color;

void main()
{
    color = vec4(v_Color, 1.0f);
}