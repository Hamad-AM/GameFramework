#include "render.h"

inline RenderTexture2D CreateTexture2D(const char* name, u32 width, u32 height, u8* data, RTextureFormat format, b32 generateMipMaps, MipmapScaling scaling, TextureWrapping wrap) {
    u32 textureID;
    RenderTexture2D texture{
        name,
        width,
        height,
        data,
        format,
        generateMipMaps,
        scaling,
        wrap
    };

    glGenTextures(1, &texture.handle.value);
    glBindTexture(GL_TEXTURE_2D, texture.handle.value);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    switch(texture.format)
    {
        case RTextureFormat::RGBA8:
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
            break;
        }
        case RTextureFormat::RGBA16F:
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texture.width, texture.height, 0, GL_RGBA, GL_FLOAT, texture.data);
            break;
        }
        case RTextureFormat::COMPRESSED:
        {
            assert(false && "No implementation of Compressed Texture BC7");
#if 0
            for (s32 i = 0; i < MAX_MIPMAP; ++i)
            {
                ImageData& data = image.imageData[i];
                glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_BPTC_UNORM, data.width, data.height, 0, data.bytes, data.data);
            }
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif
            break;
        }
        default:
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glGenerateMipmap(GL_TEXTURE_2D);
            break;
        }
    }

    switch (texture.scaling) {
        case MipmapScaling::BILINEAR:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case MipmapScaling::NEAREST_NEIGHBOUR:
            assert(false && "Not implemented nearest neighbour filtering");
            //TODO: get nearest neighbour scaling done for pixel 3D
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        default:
            // assert(false && "No scaling set");
            break;
    }

    switch (texture.wrap) {
        case TextureWrapping::REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case TextureWrapping::CLAMP:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        default:
            break;
    }

    if (texture.generateMipMaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

inline RenderTextureArray CreateTexture2DArray(s32 count, RTextureFormat format, u32 width, u32 height, MipmapScaling scaling, TextureWrapping wrap) {
    RenderTextureArray texture{"TextureArray", count, format, width, height, false, scaling, wrap};
    glGenTextures(1, &texture.handle.value);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture.handle.value);
    switch(format) {
        case RGBA8:
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
                width, height, count, 0, GL_RGBA8, GL_UNSIGNED_BYTE, nullptr);
            break;
        case DEPTH16F:
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16,
                width, height, count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            break;
        case DEPTH32F:
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
                width, height, count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            break;
        default:
            assert(false && "Not supported format for texture arrays");
            break;
    }
    switch(scaling) {
        case BILINEAR:
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        default: 
            assert(false && "Not supported scaling for texture arrays");
            break;
    }
    switch(wrap) {
        case BORDER_COLOUR: {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
            break;
        }
        default:
            assert(false && "Not supported wrapping method for texture arrays");
            break;
    }
    // Do they always have a compare mode?
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    return texture;
}

BufferHandle CreateBuffer(BufferType type, s32 vertexCount, s32 sizeElement, u8* data, DrawState state) {
    Buffer buffer{vertexCount, sizeElement, data, state, type};
    GLenum bufferType;
    GLenum glDrawType;
    switch (type){
        case ARRAY:
            bufferType = GL_ARRAY_BUFFER;
            break;
        case INDEX:
            bufferType = GL_ELEMENT_ARRAY_BUFFER;
            break;
    }
    switch (state) {
        case STATIC_DRAW:
            glDrawType = GL_STATIC_DRAW;
            break;
        default:
            assert(false && "not implemented other states for drawing yet");
            break;
    }
    glGenBuffers(1, &buffer.handle.value);
    glBindBuffer(bufferType, buffer.handle.value);
    glBufferData(bufferType, vertexCount * sizeElement, data, glDrawType);
}

VertexArray CreateVertexArray() {
    VertexArray array{};
    glGenVertexArrays(1, &array.handle.value);
    glBindVertexArray(array.handle.value);
    return array;
}

void SetVertexArrayFormat(VertexArray& array, s32 numberOfElements, VertexArrayElement* elements) {
    glBindVertexArray(array.handle.value);
    s32 totalElements = 0;
    for (s32 i = 0; i < numberOfElements; i++) {
        totalElements += elements[i].numberOfElements;
    }
    u64 stride = 0;
    for (s32 i = 0; i < numberOfElements; i++) {
        glVertexAttribPointer(i, elements[i].numberOfElements, GL_FLOAT, GL_FALSE, totalElements * sizeof(f32), (void*)stride);
        glEnableVertexAttribArray(i);
        stride += elements[i].numberOfElements * sizeof(f32);
    }
    glBindVertexArray(0);
};

Framebuffer CreateFramebuffer() {
    Framebuffer framebuffer;
    glGenFramebuffers(1, &framebuffer.fbHandle.value);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbHandle.value);
}
void FramebufferAttachColor(Framebuffer& framebuffer, RenderTexture2D& colorAttachment) {
    assert(framebuffer.textureAttachmentCount < MaxColourAttachment && "Too Many Texture Attachments");
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbHandle.value);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+framebuffer.textureAttachmentCount,GL_TEXTURE_2D , colorAttachment.handle.value, 0);
    framebuffer.colors[++framebuffer.textureAttachmentCount] = colorAttachment;
}

void FramebufferAttachDepth(Framebuffer& framebuffer, RenderTexture2D& depthAttachment) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbHandle.value);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment.handle.value, 0);
}

void FramebufferDrawAttachments(Framebuffer& framebuffer, u32* textures, u32 numberOfTextures) {
    assert(framebuffer.bDrawBuffer);
    glBindFramebuffer(1, framebuffer.fbHandle.value);
    glDrawBuffers(numberOfTextures, textures);
}

RenderBufferHandle CreateRenderbuffer(Framebuffer& framebuffer, u32 width, u32 height, DepthComp depthComp) {
    RenderBufferHandle handle;
    glBindFramebuffer(1, handle.value);

    glGenRenderbuffers(1, &handle.value);
    glBindRenderbuffer(GL_RENDERBUFFER, handle.value);
    GLenum glDepthComp;
    switch(depthComp) {
        case DepthComp::DepthComponent:
            glDepthComp = GL_DEPTH_COMPONENT;
            break;
        case DepthComp::DepthComponent24:
            glDepthComp = GL_DEPTH_COMPONENT24;
            break;
    }
    glRenderbufferStorage(GL_RENDERBUFFER, glDepthComp, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, handle.value);
    return handle;
}

void FramebufferDrawBuffer(Framebuffer& framebuffer) {
    glBindFramebuffer(1, framebuffer.fbHandle.value);
    glDrawBuffer(GL_NONE);
}

void FrameBufferNoReadBuffer(Framebuffer& framebuffer) {
    glBindFramebuffer(1, framebuffer.fbHandle.value);
    glReadBuffer(GL_NONE);
}

TextureCubemap CreateTextureCubemap(u32 width, u32 height, RTextureFormat format, MipmapScaling scaling, TextureWrapping wrap) {
    TextureCubemap cubemap;
    cubemap.width = width;
    cubemap.height = height;
    cubemap.format = format;
    cubemap.scaling = scaling;
    cubemap.wrap = wrap;

    glGenTextures(1, &cubemap.handle.value);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.handle.value);
    for (u32 j = 0; j < 6; ++j)
    {
        switch (format) {
        case RTextureFormat::DEPTH16F:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT,
                width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            break;
        case RTextureFormat::RGB16F:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_RGB16F,
                width, height, 0, GL_RGB, GL_FLOAT, NULL);
        default:
            assert(false);
        }
    }
    //TODO add options to change
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

