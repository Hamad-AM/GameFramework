#pragma once

#include <box2d/box2d.h>

#include "b2_triangles.h"
#include "b2_points.h"
#include "b2_lines.h"

using namespace atl;


namespace box2d
{
    class B2DebugDraw : public b2Draw
    {
    public:
        B2DebugDraw();
        ~B2DebugDraw();

        void Create();
        void Destroy();

        void set_proj(mat4 projection);

        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;

        void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

        void DrawTransform(const b2Transform& xf) override;

        void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

        void DrawAABB(b2AABB* aabb, const b2Color& color);

        void Flush();

        bool m_showUI;
        GLRenderPoints* m_points;
        GLRenderLines* m_lines;
        GLRenderTriangles* m_triangles;
        mat4 proj;
    };
}
