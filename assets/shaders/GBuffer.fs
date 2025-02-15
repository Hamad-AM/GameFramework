#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gMetallicRoughness;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform sampler2D albedo;
uniform sampler2D metallicRoughness;
uniform sampler2D normalMap;

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

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(FragPos, 1.0f);
    // also store the per-fragment normals into the gbuffer

    // Perturb based normals
    // vec3 N = normalize(Normal.xyz);
    // vec3 V = normalize(cameraPosition - FragPos);
    // vec3 PN = perturb_normal(N, V, TexCoords);
    // gNormal = vec4(PN, 1.0f);
    
    vec3 N = texture(normalMap, TexCoords).rgb;
    N = N * 2.0 - 1.0;
    N = normalize(TBN * N);
    gNormal = vec4(N, 1.0f);


    // and the diffuse per-fragment color
    gAlbedo = texture(albedo, TexCoords);
    // store specular intensity in gAlbedoSpec's alpha component
    gMetallicRoughness = texture(metallicRoughness, TexCoords);
}
