#version 460 core
out vec4 FragColor;

in vec2 v_TexCoords;
in vec4 FragPos;
in vec3 v_Normal;

uniform sampler2D albedo;

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

void main()
{
    vec3 albedoColor = pow(texture(albedo, v_TexCoords).rgb, vec3(2.2));
    vec3 normal = normalize(v_Normal);

    vec3 lighting = vec3(0.0);
    for(int i = 0; i < lights.length(); i++)
    {
        Light light = lights[i];
        vec3 lightPosition = vec3(light.positionX, light.positionY, light.positionZ);
        vec3 lightDirection = vec3(light.directionX, light.directionY,light.directionZ);
        vec3 lightColor = vec3(light.colorX, light.colorY, light.colorZ);
        vec3 L;
        if (light.type == Directional)
        {
            L = normalize(lightDirection);
            float diffDir = max(dot(normal, L), 0.0);
            lighting += diffDir * lightColor;
        }
        else if (light.type == Point) 
        {
            L = normalize(lightPosition - FragPos.xyz);
            // Calculate Attentuation for directional light no attenuation
            float diffPoint = max(dot(normal, L), 0.0);
            float distance    = length(lightPosition - FragPos.xyz);
            float attenuation = 1.0 / (distance * distance);
            lighting += diffPoint * lightColor * attenuation; // LightColor * attentuation lightColors[i] * attenuation;
            break;
        }
    }
    vec3 ambient = 0.3 * albedoColor;
    vec3 color = (ambient + lighting) * albedoColor;
    color = pow(color, vec3(1.0f/2.2f));
    FragColor = vec4(color, 1.0);
}
