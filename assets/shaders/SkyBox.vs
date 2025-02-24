#version 330 core
layout (location = 0) in vec3 aPos;

// uniform mat4 projection;
// uniform mat4 view;
uniform mat4 viewProj;

out vec3 localPos;

void main()
{
    localPos = aPos;

    // mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
    vec4 clipPos = viewProj * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}
