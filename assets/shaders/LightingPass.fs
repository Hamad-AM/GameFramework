#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

in vec4 FragPosLightSpace;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D gMetallicRoughness;

uniform sampler2D shadowMap;
uniform sampler2D ssao;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;

uniform vec3 viewPos;

//
// enum LightType
// {
//     None,
//     Directional,
//     Point,
//     Spotlight,
//     Area
// };
//
// struct Light
// {
//     LightType type;
//     
//     vec3 position;
//     vec3 direction;
//
//     vec3 color;
//     f32 luminance ;
//
//     f32 constant;
//     f32 linear;
//     f32 quadratic;
//
//     float cutOff;
// };



uniform vec3 lightPosition;
uniform mat4 lightSpaceMatrix;

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

float shadow_calculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
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
    if (projCoords.z <= 1.0)
    {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -2; x <= 2; ++x)
        {
            for (int y = -2; y <= 2; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 25.0f;
    }
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

    vec3 ShadowCastDirection = normalize(lightPosition - FragPos.xyz);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // for(int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPosition - FragPos.xyz);
        vec3 H = normalize(V + L);
        // Calculate Attentuation for directional light no attenuation
        // float distance    = length(lightPosition - v_WorldPos.xyz);
        // float attenuation = 1.0 / (distance * distance);
        // vec3 radiance     = vec3(1.0f) * attenuation; // LightColor * attentuation lightColors[i] * attenuation;
        vec3 radiance = vec3(10.0f);

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
        Lo += (kD * albedoColor.xyz / PI + specular) * radiance * NdotL;
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

    vec3 ambient = (kD * diffuse + specular) * 0.25;

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
    float shadow = shadow_calculation(fragPosLightSpace, N, ShadowCastDirection);
    vec3 color = (ambient + (1 - shadow) * Lo * ao);
    // color = color * 0.00000000000001;
    // color += vec3(0.0, 0.0, 0.0); //albedoColor.xyz;

    // // HDR Calculations
    // color = color / (color + vec3(1.0));
    color = ACESFilm(color);
    // // color = pow(color, vec3(0.4545f));
    color = pow(color, vec3(1.0f/2.2f));

    // vec3 lightDirNorm = normalize(lightDir);
    // float diff = max(dot(PN, lightDirNorm), 0.0);
    // // vec3 color = vec3(texture(albedo, v_TexCoords));
    // vec3 diffuse = 0.8 * diff * albedoColor.xyz;
    //
    // // Convert clip space to NDC by dividing by w, then to screen space
    // vec3 ndcPos = v_FragPosScreenSpace.xyz / v_FragPosScreenSpace.w;
    // vec2 screenSpaceCoords = ndcPos.xy * 0.5 + 0.5;  // Map to range [0, 1]
    // 
    // float AmbientOcclusion = texture(ssao, screenSpaceCoords).r;
    // vec3 ambient = 0.2 * albedoColor.xyz * AmbientOcclusion;
    //
    // float shadow = shadow_calculation(v_FragPosLightSpace, N, lightDirNorm);
    // vec3 result = ambient + (1.0 - shadow) * diffuse;

    FragColor = vec4(color, 1.0);
}
