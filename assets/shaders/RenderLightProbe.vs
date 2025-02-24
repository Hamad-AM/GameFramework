#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

uniform mat4 viewProj;
uniform mat4 model;

out vec2 v_TexCoords;
out vec3 v_Normal;
out vec4 FragPos;

void main()
{
	FragPos = viewProj * vec4(a_Position, 1.0);
    gl_Position = model * FragPos; 
    v_TexCoords = a_TexCoord;
	v_Normal = transpose(inverse(mat3(model))) * a_Normal;
}
