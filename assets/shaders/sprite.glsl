#shader vertex

#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_tex_coord;

uniform mat4 u_projection_view_mat;
uniform mat3 u_model_mat;

void
main(void)
{
    gl_Position = u_projection_view_mat * u_model_mat * vec4(a_position, 1.0f);
    v_tex_coord = a_tex_coord;
}

#shader fragment
#version 330 core

out vec4 color;

in vec2 v_tex_coord;

uniform sampler2D u_texture;

void
main(void)
{
    color = texture(u_texture, v_tex_coord);
}