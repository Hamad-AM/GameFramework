#shader vertex

#version 330 core
layout(location = 0) in vec3 a_Position;

uniform mat4 u_ProjectionViewMat;
uniform mat4 u_ModelMat;

void main()
{
    gl_Position = u_ProjectionViewMat * u_ModelMat * vec4(a_Position, 1.0f);
}

#shader fragment

#version 330 core
out vec4 color;

void main()
{
    color = vec4(1.0f);
}