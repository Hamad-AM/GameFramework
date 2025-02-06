#include "texture_atlas.h"

    void
    texture_atlas::create_sub_texture(std::string& name, ivec2& position)
    {
        sub_texture sub;
        sub.position = position;

        f32 uv[] = {
            position.x * cell_size.x, position.y * cell_size.y,
            position.x * cell_size.x + cell_size.x, position.y * cell_size.y,
            position.x * cell_size.x + cell_size.x, position.y * cell_size.y + cell_size.y,
            position.x * cell_size.x, position.y * cell_size.y + cell_size.y
        };

        for (u32 i = 0; i < 8; ++i)
        {
            if (i % 2 == 0)
                uv[i] = (f32)uv[i] / (f32)texture.height();
            else
                uv[i] = (f32)uv[i] / (f32)texture.width();
        }

        f32 vertices[] = {
            // positions          // texture coords
            -0.5f, -0.5f, 0.0f,   uv[0], uv[1], // bottom left
            0.5f, -0.5f, 0.0f,   uv[2], uv[3], // bottom right
            0.5f,  0.5f, 0.0f,   uv[4], uv[5], // top right
            -0.5f,  0.5f, 0.0f,   uv[6], uv[7]  // top left 
        };
        u32 indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &sub.va);
        glGenBuffers(1, &sub.vb);
        glGenBuffers(1, &sub.ib);

        glBindVertexArray(sub.va);

        glBindBuffer(GL_ARRAY_BUFFER, sub.vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub.ib);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        sub_textures[name] = sub;
    }
