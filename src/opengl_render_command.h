#ifndef RENDER_OPENGL_HH
#define RENDER_OPENGL_HH

#include "math.h"
#include "sprite.h"
#include "camera.h"

class OpenGlRenderCommand
{
public:
    void
    BeginDraw2D(Camera* camera);

    void
    DrawSprite(Sprite* sprite, v3* position, Shader* shader);

    void
    DeleteTexture2D(Texture2D* texture);
private:
    Camera* camera_;
};

#endif