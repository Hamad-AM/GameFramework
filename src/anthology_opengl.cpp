#include "anthology_opengl.h"

#include "anthology_renderer.h"

inline void
gl_draw_texture(Texture2D* texture, v2 position, Camera* camera)
{
    if (!texture->initialized)
    {

    }
}

inline void
gl_draw_model(Model* model, v3* position, Camera* camera)
{

}

inline void
gl_delete_texture(Texture2D* texture)
{

}

void
gl_render_to_output(RenderGroup* render_group)
{
    for (u32 base_address = 0;
        base_address < group->push_buffer_size;
        base_address += sizeof(RenderElement))
    {
        RenderElement* render_element = (RenderElement*)(group->push_buffer_head + base_address);
        switch(render_element->type)
        {
            case RenderType_Texture:
            {
                gl_draw_texture((Texture2D*)render_element->element, v3_xy(&render_element->position), group->ortho_camera);
            }
                break;
            case RenderType_Model:
            {
                gl_draw_model((Model*)render_element->element, render_element->position, group->persp_camera);
            }
                break;
        }
    }
}