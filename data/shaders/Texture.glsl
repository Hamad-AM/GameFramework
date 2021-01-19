#shader vertex

#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec4 v_VertexPos;

uniform mat4 u_ProjectionViewMat;
uniform mat4 u_ModelMat;

void main()
{
    gl_Position = u_ProjectionViewMat * u_ModelMat * vec4(a_Position, 1.0f);
    v_TexCoord = a_TexCoord;
    v_Normal = mat3(transpose(inverse(u_ModelMat))) * a_Normal;
    v_VertexPos = u_ModelMat * vec4(a_Position, 1.0f);
}

#shader fragment

#version 330 core

struct Material
{
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct DirLight 
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight u_DirLight;

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 view_direction);

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NO_POINT_LIGHTS 4
uniform PointLight u_PointLights[NO_POINT_LIGHTS];

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction);

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight u_SpotLight;

vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 view_direction);

out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec4 v_VertexPos;

uniform vec3 u_ViewPosition;

void main()
{
    vec3 view_direction = normalize(u_ViewPosition - v_VertexPos.xyz);
    vec3 normal = normalize(v_Normal);

    vec3 light_result = calculate_dir_light(u_DirLight, normal, view_direction);

    for (int i = 0; i < NO_POINT_LIGHTS; ++i)
        light_result += calculate_point_light(u_PointLights[i], normal, v_VertexPos.xyz, view_direction);
    
    light_result += calculate_spot_light(u_SpotLight, normal, view_direction);


    color = vec4(light_result, 1.0f);
}

vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 view_direction)
{
    vec3 light_direction = normalize(light.position - v_VertexPos.xyz);
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    
    if (theta > light.innerCutOff)
    {
        float light_influence = max(dot(light_direction, normalize(v_Normal)), 0.0);
        vec3 diffuse_light = light.diffuse * light_influence * vec3(texture(material.diffuse, v_TexCoord).xyz);

        vec3 reflect_direction = reflect(-light_direction, v_Normal); 
        float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
        vec3 specular_light = light.specular * material.specular * spec;

        vec3 ambient_light = light.ambient * vec3(texture(material.diffuse, v_TexCoord).xyz);

        diffuse_light *= intensity;
        specular_light *= intensity;

        return diffuse_light + specular_light + ambient_light;
    }
    else 
        return light.ambient * vec3(texture(material.diffuse, v_TexCoord).xyz);
}

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 view_direction)
{
    vec3 light_direction = normalize(-light.direction);
    float light_influence = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_light = light.diffuse * light_influence * texture(material.diffuse, v_TexCoord).rgb;

    vec3 reflect_direction = reflect(-light_direction, normal); 
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular_light = light.specular * material.specular * spec;

    vec3 ambient_light = light.ambient * vec3(texture(material.diffuse, v_TexCoord).xyz);

    return (ambient_light + diffuse_light + specular_light);
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction)
{
    vec3 light_direction = normalize(light.position - v_VertexPos.xyz);
    float light_influence = max(dot(light_direction, normalize(v_Normal)), 0.0);

    vec3 reflect_direction = reflect(-light_direction, v_Normal); 
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuse_light = light.diffuse * light_influence * vec3(texture(material.diffuse, v_TexCoord).xyz);
    vec3 specular_light = light.specular * material.specular * spec;
    vec3 ambient_light = light.ambient * vec3(texture(material.diffuse, v_TexCoord).xyz);

    return (ambient_light + specular_light + diffuse_light);
}