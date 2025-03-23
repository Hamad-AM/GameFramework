#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); 

uniform mat4 projection;
uniform mat4 view;

// float horizon_occlusion(vec3 viewPos, vec3 viewNormal, vec2 deltaUV, float numSamples) {
//     float occlusion = 0.0;
//     vec3 sliceDir = normalize(vec3(deltaUV, 0.0));
//     
//     // Make sure slice direction is perpendicular to normal
//     sliceDir = normalize(sliceDir - dot(sliceDir, viewNormal) * viewNormal);
//     
//     // Create a basis with the normal and slice direction
//     vec3 orthoDir = cross(viewNormal, sliceDir);
//     
//     // Create a view-to-tangent transformation matrix
//     mat3 TBN = mat3(sliceDir, orthoDir, viewNormal);
//     
//     // Initialize horizon angles
//     float horizonAngleMax = -1.0;
//     float horizonAngleMin = 1.0;
//     
//     // Step along the direction and find the horizon angle
//     for (int i = 1; i <= numSamples; i++) {
//         // Calculate sample position
//         float sampleDist = radius * (float(i) / float(numSamples));
//         vec2 sampleUV = TexCoords + deltaUV * sampleDist;
//         
//         // Get sample view position
//         vec3 sampleViewPos = getViewPos(sampleUV);
//         
//         // Calculate direction to sample
//         vec3 v = sampleViewPos - viewPos;
//         float vLength = length(v);
//         v = v / vLength;
//         
//         // Check if sample is within radius
//         if (vLength < radius) {
//             // Calculate horizon angle
//             float horizonAngle = dot(v, viewNormal);
//             horizonAngle = asin(clamp(horizonAngle, -1.0, 1.0));
//             
//             // Update horizon angles
//             horizonAngleMax = max(horizonAngleMax, horizonAngle);
//             horizonAngleMin = min(horizonAngleMin, horizonAngle);
//         }
//     }
//     
//     // Calculate occlusion from horizon angles
//     float horizonOcclusion = max(0.0, sin(horizonAngleMax) - sin(horizonAngleMin));
//     return horizonOcclusion;
// }
//
// mat2 getRandomRotationMatrix(vec2 uv) {
//     // vec2 noise = texture(gNoise, uv * screenSize / vec2(4.0)).xy;
//     vec3 randomVec = normalize(texture(texNoise, uv * noiseScale).xyz);
//     noise = noise * 2.0 - 1.0;
//     float cosTheta = noise.x;
//     float sinTheta = noise.y;
//     return mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
// }


void main()
{
    // // get input for SSAO algorithm
    // //TODO: Fix this weird stuff
    // vec3 fragPos = texture(positionBuffer, TexCoords).xyz;
    // vec4 viewSpacefragPos = view * vec4(fragPos, 1);
    // fragPos = viewSpacefragPos.xyz;
    //
    // vec3 normal = normalize(texture(normalBuffer, TexCoords).rgb);
    // mat2 rotMat = getRandomRotationMatrix(TexCoords);
    // // create TBN change-of-basis matrix: from tangent-space to view-space
    // vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    // vec3 bitangent = cross(normal, tangent);
    // mat3 TBN = mat3(tangent, bitangent, normal);
    // // iterate over the sample kernel and calculate occlusion factor
    //
    // int numDirections = 4;
    // int numSamples = 4;
    //
    // float occlusion = 0.0;
    //
    //     // Integrate occlusion over directions
    // for (int i = 0; i < numDirections; i++) {
    //     // Calculate sampling direction
    //     float angle = (float(i) + 0.5) * PI / float(numDirections);
    //     vec2 direction = vec2(cos(angle), sin(angle));
    //     
    //     // Apply random rotation to reduce banding
    //     direction = rotMat * direction;
    //     
    //     // Scale direction based on screen aspect ratio to avoid stretching
    //     direction *= vec2(720 / 1280, 1.0);
    //     
    //     // Normalize the direction
    //     direction = normalize(direction);
    //     
    //     // Calculate the delta UV for this direction
    //     vec2 deltaUV = direction * radius / (vec2(1280, 720) * float(numSamples));
    //     
    //     // Compute occlusion for this direction
    //     occlusion += horizon_occlusion(fragPos, TBN * normal, deltaUV, numSamples);
    // }
    // 
    // // Average the occlusion over all directions
    // occlusion /= float(numDirections);
    // 
    // // Apply bias and normalize
    // occlusion = pow(1.0 - occlusion, bias);
    //
    // get input for SSAO algorithm
    //TODO: Fix this weird stuff
    vec3 fragPos = texture(positionBuffer, TexCoords).xyz;
    vec4 viewSpacefragPos = view * vec4(fragPos, 1);
    fragPos = viewSpacefragPos.xyz;

    vec3 normal = normalize(texture(normalBuffer, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        vec3 worldSpaceSamplePosition = texture(positionBuffer, offset.xy).xyz; // get depth value of kernel sample
        viewSpacefragPos = view * vec4(worldSpaceSamplePosition, 1);
        float sampleDepth = viewSpacefragPos.z;
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    // FragColor = occlusion;
    FragColor = vec4(occlusion, 0.0f, 0.0f, 1.0f);
}


