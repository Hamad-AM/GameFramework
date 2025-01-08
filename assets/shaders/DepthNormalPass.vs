#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragNormal;
out vec4 fragPosition;

void main() {
    vec4 worldPos = model * vec4(position, 1.0);
    fragPosition = view * worldPos;
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    fragNormal = normalMatrix * normal;
    gl_Position = projection * view * worldPos;
}
