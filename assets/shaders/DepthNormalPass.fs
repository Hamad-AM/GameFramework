#version 330 core
layout (location = 0) out vec4 normalBuffer;
layout (location = 1) out vec4 positionBuffer;

//out vec4 color;

in vec3 fragNormal;
in vec4 fragPosition;

void main() {
    normalBuffer = vec4(normalize(fragNormal), 1.0f);
    positionBuffer = fragPosition;
    // normalBuffer = normal * 0.5 + 0.5;
//    normalBuffer = vec3(1.0f, 0.0f, 0.0f);
//    color = vec4(normalBuffer, 1.0f);
}
