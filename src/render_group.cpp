#include "render_group.h"

#include "malloc.h"

static RenderGroup
RenderGroup::Allocate(u64 size)
{
    RenderGroup group = {};

    group.max_push_buffer_size_ = size;
    group.push_buffer_size_ = 0;
    group.push_buffer_head_ = (u8*)malloc(size);

    return group;
}

void
RenderGroup::DrawTexture(Texture2D* texture, v3* position)
{
    RenderElement* render_element = (RenderElement*)push_render_element(group);
    position_ = *position;
    type_ = RenderType_Texture;
    element_ = (void*)texture;
}

void
RenderGroup::Model(Model* model, v3* position)
{}

void*
PushRenderElement()
{
    void* element = 0;

    u32 size = sizeof(RenderElement);

    if ((push_buffer_size_ + size) < max_push_buffer_size_)
    {
        element = push_buffer_head_ + push_buffer_size_;
        push_buffer_size_ += size;
    }
    else 
    {
        assert(true);
    }

    return element;
}

void
RenderToOutput()
{
    OpenGLRenderToOutput(this);
}