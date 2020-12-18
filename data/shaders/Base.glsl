#shader vertex

#version 330 core
layout(location = 0) in vec3 a_Position;

out vec3 v_Position;
out vec4 v_Color;

void main()
{
    v_Position = a_Position;
    v_Color = vec4(a_Position, 1.0);
    gl_Position = vec4(a_Position, 1.0f);
}

#shader fragment

#version 330 core
layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Color;

void main()
{
    color = vec4(v_Position * 0.5 + 0.5, 1.0);
    color = v_Color;
}