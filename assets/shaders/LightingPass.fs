#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D gMetallicRoughness;

uniform sampler2DArray shadowMap;
uniform sampler2D ssao;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;

uniform vec3 viewPos;

#define Directional 1
#define Point 2
#define Spotlight 3
#define Area 4

struct Light
{
    int type;
    
    float positionX;
    float positionY;
    float positionZ;
    float directionX;
    float directionY;
    float directionZ;

    float colorX;
    float colorY;
    float colorZ;
    float luminance;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    int isShadowCasting;
    int shadowIndex;
};

layout(std430, binding=0) buffer Lights {
    Light lights[];
};

layout(std140, binding=1) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

uniform samplerCube depthCubemaps[4];
uniform float pointShadowFarPlane;


const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float shadow_calculation(vec3 fragPosWorldSpace, vec3 normal, vec3 lightDir)
{
    float depthValue = abs(viewPos.z);
    int layer = -1;
    for (int i = 0; i < cascadeCount-1; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount-1;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0f);
    // Perspective Divide, returns lgith space position in [-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Change range to [0, 1] to sample depthMap
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    // float bias = 0.005;
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float slope = length(vec2(dFdx(currentDepth), dFdy(currentDepth)));
    float bias = 0.005 * slope;
    float shadow = 0.0f;
    float pcfDepth;
    if (projCoords.z <= 1.0)
    {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
        for (int x = -4; x <= 4; ++x)
        {
            for (int y = -4; y <= 4; ++y)
            {
                pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 81.0f;
    }
    return projCoords.y
    ;
}

float PointShadowCalculation(vec3 fragPos, vec3 lightPos, int shadowMapIndex)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float bias = 0.05;
    int sampleCount = 20;  // Number of samples for PCF
    float shadow = 0.0;

    float diskRadius = 0.05;  // Control softness

    for(int i = 0; i < sampleCount; i++)
    {
        // Generate a small random offset (a simple Poisson Disk pattern)
        vec3 offset = diskRadius * vec3(
            cos(6.2831853 * float(i) / float(sampleCount)),
            sin(6.2831853 * float(i) / float(sampleCount)),
            0.0
        );
        
        float closestDepth = texture(depthCubemaps[0], fragToLight + offset).r;
        closestDepth *= pointShadowFarPlane; // Convert from [0,1] depth range

        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(sampleCount); // Average result
    return shadow;
}

vec3 ACESFilm(vec3 color)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return (color*(a*color+b))/(color*(c*color+d)+e);
}

vec3 Reinhard(vec3 color)
{
    return pow(color, vec3(1.0/2.2));
}

void main()
{
    float alpha = texture(gAlbedo, TexCoords).a;
    vec3 albedoColor = pow(texture(gAlbedo, TexCoords).rgb, vec3(2.2));
    if (alpha < 0.1) discard;
    vec4 metallicRoughnessValues = texture(gMetallicRoughness, TexCoords);
    // float metallic = metallicRoughnessValues.b;
    // float roughness = metallicRoughnessValues.g;
    float metallic = clamp((metallicRoughnessValues.g - 0.04) / (1.0-0.04), 0.0, 1.0);
    float roughness = 1.0 - metallicRoughnessValues.b;

    vec3 FragPos = texture(gPosition, TexCoords).rgb;

    // vec3 ndcPos = v_FragPosScreenSpace.xyz / v_FragPosScreenSpace.w;
    // vec2 screenSpaceCoords = ndcPos.xy * 0.5 + 0.5;  // Map to range [0, 1]
    // float ao = texture(ssao, screenSpaceCoords).r;
    float ao = texture(ssao, TexCoords).r;

    vec3 V = normalize(viewPos - FragPos.xyz);
    vec3 N = texture(gNormal, TexCoords).xyz;
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoColor.xyz, metallic);

    // vec3 lPos = vec3(lights[0].positionX, lights[0].positionY, lights[0].positionZ);
    // vec3 ShadowCastDirection = normalize(lPos - FragPos.xyz);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lights.length(); i++)
    {
        Light light = lights[i];
        vec3 lightPosition = vec3(light.positionX, light.positionY, light.positionZ);
        vec3 lightDirection = vec3(light.directionX, light.directionY,light.directionZ);
        vec3 lightColor = vec3(light.colorX, light.colorY, light.colorZ);
        // calculate per-light radiance
        vec3 radiance, L, H;
        if (light.type == Directional)
        {
            L = normalize(lightDirection);
            H = normalize(V + L);
            radiance = lightColor * light.luminance;
        }
        else if (light.type == Point) 
        {
            L = normalize(lightPosition - FragPos.xyz);
            H = normalize(V + L);
            // Calculate Attentuation for directional light no attenuation
            float distance    = length(lightPosition - FragPos.xyz);
            float attenuation = 1.0 / (distance * distance);
            radiance          = lightColor * light.luminance * attenuation; // LightColor * attentuation lightColors[i] * attenuation;
        }

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        float shadow = 0;
        if (light.isShadowCasting == 1)
        {
            if (light.type == Directional)
            {
                shadow = shadow_calculation(FragPos, N, L);
            }
            else if (light.type == Point)
            {
                shadow = PointShadowCalculation(FragPos, lightPosition, light.shadowIndex);
                continue;
            }
        }
        Lo += shadow; //(1-shadow) * (kD * albedoColor.xyz / PI + specular) * radiance * NdotL;
    }

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedoColor.xyz;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = (ambient + Lo);

    // // HDR Calculations
    color = ACESFilm(color);
    color = pow(color, vec3(1.0f/2.2f));

    if (lights[0].type == Directional)
    {
        color = vec3(Lo);
    }

    FragColor = vec4(color, alpha);
}
