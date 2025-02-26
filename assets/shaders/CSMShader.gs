#version 460 core
    
layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;
    
layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

void main()
{          
    for (int cascade = 0; cascade < 4; cascade++)
    {
        gl_Layer = cascade;
        for (int i = 0; i < 3; ++i)
        {
            gl_Position = 
                lightSpaceMatrices[cascade] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
