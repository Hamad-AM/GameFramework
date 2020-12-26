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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct Light
{
    vec3 position;
    vec3 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec4 v_VertexPos;

uniform sampler2D u_Texture;
uniform vec3 u_ViewPosition;

void main()
{
    vec3 light_direction = normalize(light.position - v_VertexPos.xyz);
    float light_influence = max(dot(light_direction, normalize(v_Normal)), 0.0);
    vec3 diffuse_light = light.diffuse * (light_influence * material.diffuse) * light.color;


    vec3 view_direction = normalize(u_ViewPosition - v_VertexPos.xyz);
    vec3 reflect_direction = reflect(-light_direction, v_Normal); 
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular_light = light.specular * material.specular * spec * light.color;

    vec3 ambient_light = light.ambient * material.ambient * light.color;

    color = texture(u_Texture, v_TexCoord) * vec4(specular_light + ambient_light + diffuse_light, 1.0f);
}