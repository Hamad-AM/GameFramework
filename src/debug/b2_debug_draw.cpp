#include "b2_debug_draw.h"

namespace box2d
{
    //
    B2DebugDraw::B2DebugDraw()
    {
        m_showUI = true;
        m_points = NULL;
        m_lines = NULL;
        m_triangles = NULL;
    }

    //
    B2DebugDraw::~B2DebugDraw()
    {
        b2Assert(m_points == NULL);
        b2Assert(m_lines == NULL);
        b2Assert(m_triangles == NULL);
    }

    //
    void B2DebugDraw::Create()
    {
        m_points = new GLRenderPoints;
        m_points->Create();
        m_lines = new GLRenderLines;
        m_lines->Create();
        m_triangles = new GLRenderTriangles;
        m_triangles->Create();
    }

    //
    void B2DebugDraw::Destroy()
    {
        m_points->Destroy();
        delete m_points;
        m_points = NULL;

        m_lines->Destroy();
        delete m_lines;
        m_lines = NULL;

        m_triangles->Destroy();
        delete m_triangles;
        m_triangles = NULL;
    }
    void B2DebugDraw::set_proj(mat4 projection)
    {
        proj = projection;
        m_points->set_proj(proj);
        m_lines->set_proj(proj);
        m_triangles->set_proj(proj);
    }

    //
    void B2DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        b2Vec2 p1 = vertices[vertexCount - 1];
        for (int32 i = 0; i < vertexCount; ++i)
        {
            b2Vec2 p2 = vertices[i];
            m_lines->Vertex(p1, color);
            m_lines->Vertex(p2, color);
            p1 = p2;
        }
    }

    //
    void B2DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

        for (int32 i = 1; i < vertexCount - 1; ++i)
        {
            m_triangles->Vertex(vertices[0], fillColor);
            m_triangles->Vertex(vertices[i], fillColor);
            m_triangles->Vertex(vertices[i + 1], fillColor);
        }

        b2Vec2 p1 = vertices[vertexCount - 1];
        for (int32 i = 0; i < vertexCount; ++i)
        {
            b2Vec2 p2 = vertices[i];
            m_lines->Vertex(p1, color);
            m_lines->Vertex(p2, color);
            p1 = p2;
        }
    }

    //
    void B2DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
    {
        const float k_segments = 16.0f;
        const float k_increment = 2.0f * b2_pi / k_segments;
        float sinInc = sinf(k_increment);
        float cosInc = cosf(k_increment);
        b2Vec2 r1(1.0f, 0.0f);
        b2Vec2 v1 = center + radius * r1;
        for (int32 i = 0; i < k_segments; ++i)
        {
            // Perform rotation to avoid additional trigonometry.
            b2Vec2 r2;
            r2.x = cosInc * r1.x - sinInc * r1.y;
            r2.y = sinInc * r1.x + cosInc * r1.y;
            b2Vec2 v2 = center + radius * r2;
            m_lines->Vertex(v1, color);
            m_lines->Vertex(v2, color);
            r1 = r2;
            v1 = v2;
        }
    }

    //
    void B2DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
    {
        const float k_segments = 16.0f;
        const float k_increment = 2.0f * b2_pi / k_segments;
        float sinInc = sinf(k_increment);
        float cosInc = cosf(k_increment);
        b2Vec2 v0 = center;
        b2Vec2 r1(cosInc, sinInc);
        b2Vec2 v1 = center + radius * r1;
        b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
        for (int32 i = 0; i < k_segments; ++i)
        {
            // Perform rotation to avoid additional trigonometry.
            b2Vec2 r2;
            r2.x = cosInc * r1.x - sinInc * r1.y;
            r2.y = sinInc * r1.x + cosInc * r1.y;
            b2Vec2 v2 = center + radius * r2;
            m_triangles->Vertex(v0, fillColor);
            m_triangles->Vertex(v1, fillColor);
            m_triangles->Vertex(v2, fillColor);
            r1 = r2;
            v1 = v2;
        }

        r1.Set(1.0f, 0.0f);
        v1 = center + radius * r1;
        for (int32 i = 0; i < k_segments; ++i)
        {
            b2Vec2 r2;
            r2.x = cosInc * r1.x - sinInc * r1.y;
            r2.y = sinInc * r1.x + cosInc * r1.y;
            b2Vec2 v2 = center + radius * r2;
            m_lines->Vertex(v1, color);
            m_lines->Vertex(v2, color);
            r1 = r2;
            v1 = v2;
        }

        // Draw a line fixed in the circle to animate rotation.
        b2Vec2 p = center + radius * axis;
        m_lines->Vertex(center, color);
        m_lines->Vertex(p, color);
    }

    //
    void B2DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        m_lines->Vertex(p1, color);
        m_lines->Vertex(p2, color);
    }

    //
    void B2DebugDraw::DrawTransform(const b2Transform& xf)
    {
        const float k_axisScale = 0.4f;
        b2Color red(1.0f, 0.0f, 0.0f);
        b2Color green(0.0f, 1.0f, 0.0f);
        b2Vec2 p1 = xf.p, p2;

        m_lines->Vertex(p1, red);
        p2 = p1 + k_axisScale * xf.q.GetXAxis();
        m_lines->Vertex(p2, red);

        m_lines->Vertex(p1, green);
        p2 = p1 + k_axisScale * xf.q.GetYAxis();
        m_lines->Vertex(p2, green);
    }

    //
    void B2DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
    {
        m_points->Vertex(p, color, size);
    }

    //
    void B2DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
    {
        b2Vec2 p1 = aabb->lowerBound;
        b2Vec2 p2 = b2Vec2(aabb->upperBound.x, aabb->lowerBound.y);
        b2Vec2 p3 = aabb->upperBound;
        b2Vec2 p4 = b2Vec2(aabb->lowerBound.x, aabb->upperBound.y);

        m_lines->Vertex(p1, c);
        m_lines->Vertex(p2, c);

        m_lines->Vertex(p2, c);
        m_lines->Vertex(p3, c);

        m_lines->Vertex(p3, c);
        m_lines->Vertex(p4, c);

        m_lines->Vertex(p4, c);
        m_lines->Vertex(p1, c);
    }

    //
    void B2DebugDraw::Flush()
    {
        m_triangles->Flush();
        m_lines->Flush();
        m_points->Flush();
    }

}