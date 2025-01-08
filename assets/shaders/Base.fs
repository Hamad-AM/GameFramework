#version 330 core

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform sampler2D shadowMap;
uniform sampler2D ssao;
uniform sampler2D albedo;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughness;


in vec4 v_Normal;
in vec4 v_WorldPos;
in vec4 v_ViewVec;
in vec2 v_TexCoords;
in vec4 v_FragPosLightSpace;
in vec4 v_FragPosScreenSpace;

const float PI = 3.14159265359;

mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv ) { // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
    // solve the linear system 
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
    // construct a scale-invariant frame
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    vec3 map = texture(normalMap, texcoord ).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
}

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

float shadow_calculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Perspective Divide, returns lgith space position in [-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Change range to [0, 1] to sample depthMap
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    // float bias = 0.005;
    float bias = 0.005; //max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0f;
    if (projCoords.z <= 1.0)
    {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0f;
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
    float alpha = texture(albedo, v_TexCoords).a;
    vec3 albedoColor = pow(texture(albedo, v_TexCoords).rgb, vec3(2.2));
    if (alpha < 0.1) discard;
    vec4 metallicRoughnessValues = texture(metallicRoughness, v_TexCoords);
    float metallic = metallicRoughnessValues.b;
    float roughness = metallicRoughnessValues.g;

    vec3 ndcPos = v_FragPosScreenSpace.xyz / v_FragPosScreenSpace.w;
    vec2 screenSpaceCoords = ndcPos.xy * 0.5 + 0.5;  // Map to range [0, 1]
    float ao = texture(ssao, screenSpaceCoords).r;

    vec3 N = normalize(v_Normal.xyz);
    vec3 V = normalize(cameraPosition - v_WorldPos.xyz);
    vec3 PN = perturb_normal(N, V, v_TexCoords);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoColor.xyz, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // for(int i = 0; i < 4; ++i)
    // {
    // calculate per-light radiance
    vec3 L = normalize(lightPosition - v_WorldPos.xyz);
    vec3 H = normalize(V + L);
    // Calculate Attentuation for directional light no attenuation
    // float distance    = length(lightPosition - v_WorldPos.xyz);
    // float attenuation = 1.0 / (distance * distance);
    // vec3 radiance     = vec3(1.0f) * attenuation; // LightColor * attentuation lightColors[i] * attenuation;
    vec3 radiance = vec3(10.0f);

    // cook-torrance brdf
    float NDF = DistributionGGX(PN, H, roughness);
    float G   = GeometrySmith(PN, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(PN, V), 0.0) * max(dot(PN, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;

    // add to outgoing radiance Lo
    float NdotL = max(dot(PN, L), 0.0);
    Lo += (kD * albedoColor.xyz / PI + specular) * radiance * NdotL;
    // }

    vec3 ambient = vec3(0.25) * albedoColor.xyz * ao;
    float shadow = shadow_calculation(v_FragPosLightSpace, N, L);
    vec3 color = ambient + (1.0 - shadow) * Lo;

    // HDR Calculations
    color = color / (color + vec3(1.0));
    color = ACESFilm(color);
    color = pow(color, vec3(0.4545f));


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

    FragColor = vec4(color, alpha);
}
