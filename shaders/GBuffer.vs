#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec3 T;
out vec3 B;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;
	
    vec4 n = model * vec4(aNormal, 1.0f);
    Normal = n.xyz;

    // vec4 T = normalize(model * vec4(aTangent, 1.0f));
    T = normalize(model * vec4(aTangent, 1.0f)).xyz;
    vec3 N = normalize(Normal);
    // vec3 B = normalize(cross(N, T.xyz));
    B = normalize(cross(N, T.xyz));
    TBN = mat3(T.xyz, B, N);


    gl_Position = projection * view * worldPos;
}
