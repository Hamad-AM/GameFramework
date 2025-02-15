#version 330 core

out vec4 FragColor;

in vec2 TexCoord;   // Incoming texture coordinates from vertex shader

uniform vec3 lightColor;

void main() {
    FragColor = vec4(lightColor, 1.0f);  // Sample texture
}
