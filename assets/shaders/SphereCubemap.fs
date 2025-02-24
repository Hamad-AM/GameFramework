#version 330 core

// Input from vertex shader
in vec3 fragPos;       // Fragment position in world space
in vec3 fragNormal;    // Normal vector for lighting
in vec2 texCoords;
in vec3 sphereCenter;

// Output color
out vec4 FragColor;

// Uniforms
uniform samplerCube cubemap;  // Cubemap texture (the skybox)
uniform vec3 cameraPos;      // Camera position (to compute view direction)

void main()
{
    // Normalize the normal vector
    vec3 normalizedNormal = normalize(fragNormal);
    
    // Compute the direction to sample the cubemap (view direction)
    vec3 viewDir = normalize(fragPos - cameraPos);

    vec3 tex = normalize(fragPos - sphereCenter);

    // Sample the cubemap
    vec4 cubemapColor = texture(cubemap, tex);

    // Output the color (you could apply lighting or other effects here)
    FragColor = cubemapColor;
}

