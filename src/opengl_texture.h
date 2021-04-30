#ifndef OPENGL_TEXTURE_HH
#define OPENGL_TEXTURE_HH

#include "texture.h"
#include <glad/glad.h>

class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(const char* file_path, TextureFilter filter, TextureWrap wrapping, TextureType type);
    ~OpenGLTexture2D();

    void
    initialize(u32 slot);

    u32
    id() { return id_; }

    u32
    slot() { return slot_; }

    b32
    initialized() { return initialized_; }

    static GLenum
    ConvertToGLWrapping(TextureWrap wrap);
    
    static GLenum
    ConvertToGLFilter(TextureFilter filter);

private:
    u32 id_;
    u32 slot_;
    bool initialized_;
};

#endif