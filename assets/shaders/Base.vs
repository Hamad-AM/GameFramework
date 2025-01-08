#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
//uniform vec3 lightPos;

out vec4 v_Normal;
//out vec3 v_LightPos;
out vec4 v_WorldPos;
out vec4 v_ViewVec;
out vec2 v_TexCoords;
out vec4 v_FragPosLightSpace;
out vec4 v_FragPosScreenSpace;

void main()
{
    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * vec4(a_Position, 1.0f);
    v_Normal = model * vec4(a_Normal, 1.0f);
    v_TexCoords = a_TexCoord;
    v_WorldPos = model * vec4(a_Position, 1.0f);
    //v_LightPos = lightPos - v_ViewVec;
    v_ViewVec = view[3] - v_WorldPos;

    v_FragPosLightSpace = lightSpaceMatrix * model * vec4(a_Position, 1.0f);
    v_FragPosScreenSpace = gl_Position;
}
