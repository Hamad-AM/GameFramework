#pragma once

#include <box2d/box2d.h>
#include <glad/glad.h>

#include "../gmath.h"
using namespace atl;

namespace box2d
{
    struct GLRenderLines
    {
        void Create();

        void Destroy();

        void Vertex(const b2Vec2& v, const b2Color& c);

        void Flush();

        void set_proj(mat4 projection) { proj = projection; }
        mat4 proj;

        enum { e_maxVertices = 2 * 512 };
        b2Vec2 m_vertices[e_maxVertices];
        b2Color m_colors[e_maxVertices];

        int32 m_count;

        GLuint m_vaoId;
        GLuint m_vboIds[2];
        GLuint m_programId;
        GLint m_projectionUniform;
        GLint m_vertexAttribute;
        GLint m_colorAttribute;
    };
}