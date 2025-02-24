#version 330 core

// Input attributes
layout(location = 0) in vec3 aPosition;  // Vertex position
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;    // Vertex normal

// Output to fragment shader
out vec3 fragPos;     // Fragment position in world space
out vec3 fragNormal;  // Normal vector for lighting
out vec2 texCoords;
out vec3 sphereCenter;

// Uniforms
uniform mat4 model;   // Model matrix (for positioning the sphere)
uniform mat4 view;    // View matrix (camera)
uniform mat4 projection; // Projection matrix (for perspective)

void main()
{
    // Transform the vertex position to world space
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    sphereCenter = model[3].xyz;
    
    // Pass the world position and normal to the fragment shader
    fragPos = worldPosition.xyz;
    fragNormal = normalize(mat3(transpose(inverse(model))) * aNormal);

	texCoords = aTexCoords;

    // Final position of the vertex after applying model, view, and projection matrices
    gl_Position = projection * view * worldPosition;
}
