#pragma once

#include "../common.h"

namespace atl
{
    typedef vertex_array u32;
    typedef index_buffer u32;
    typedef vertex_buffer u32;

    enum draw_state
    {
        STATIC_BUFFER,
        DYNAMIC_BUFFER
    };

    class gfx
    {
    public:
        static vertex_array create_vertex_array();
        static vertex_buffer create_vertex_buffer();
        static index_buffer create_index_buffer();

        template<typename T>
        static void send_vertex_buffer_data(const vertex_buffer& vb, T* data, u32 size, );
        static void send_index_buffer_data();
        static void set_vertex_array_layout();

    };
}