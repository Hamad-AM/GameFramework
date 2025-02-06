#version 330 core
out vec4 FragColor;

in vec3 localPos;
  
uniform samplerCube environmentMap;
  
vec3 ACESFilm(vec3 color)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return (color*(a*color+b))/(color*(c*color+d)+e);
}

void main()
{
    vec3 envColor = texture(environmentMap, localPos).rgb;
    
    // envColor = envColor / (envColor + vec3(1.0));
    // envColor = ACESFilm(envColor);
    // envColor = pow(envColor, vec3(0.4545)); 
    envColor = pow(envColor, vec3(1.0f/2.2f));

    FragColor = vec4(envColor, 1.0);
}
