#include "render.h"

#include <iostream>

#define DSA

GLenum glDrawState(DrawState state) {
    GLenum result;
    switch (state) {
        case STATIC_DRAW:
            result = GL_STATIC_DRAW;
            break;
        default:
            assert(false && "not implemented other states for drawing yet");
            break;
    }
    return result;
}

void TextureFormat2GL(RTextureFormat format, GLenum* formatType, GLenum* internalFormat, GLenum* type) {
    switch (format) {
        case RTextureFormat::R8:
            *formatType = GL_R8;
            *internalFormat = GL_R8;
            *type = GL_UNSIGNED_BYTE;
            break;
        case RTextureFormat::RG16F:
            *formatType = GL_RG;
            *internalFormat = GL_RG16F;
            *type = GL_FLOAT;
            break;
        case RTextureFormat::RGB8:
            *formatType = GL_RGB;
            *internalFormat = GL_RGB;
            *type = GL_UNSIGNED_BYTE;
            break;
        case RTextureFormat::RGB16F:
            *formatType = GL_RGB;
            *internalFormat = GL_RGB16F;
            *type = GL_FLOAT;
            break;
        case RTextureFormat::RGB32F:
            *formatType = GL_RGB;
            *internalFormat = GL_RGB32F;
            *type = GL_FLOAT;
            break;
        case RTextureFormat::RGBA8:
            *formatType = GL_RGBA;
            *internalFormat = GL_RGBA;
            *type = GL_UNSIGNED_BYTE;
            break;
        case RTextureFormat::RGBA16F:
            *formatType = GL_RGBA;
            *internalFormat = GL_RGBA16F;
            *type = GL_FLOAT;
            break;
        case RTextureFormat::RGBA32F:
            *formatType = GL_RGBA;
            *internalFormat = GL_RGBA32F;
            *type = GL_FLOAT;
            break;
        case RTextureFormat::DEPTH16F:
            *formatType = GL_DEPTH_COMPONENT;
            *internalFormat = GL_DEPTH_COMPONENT;
            *type = GL_FLOAT;
            break;
        case RTextureFormat::DEPTH32F:
            *formatType = GL_DEPTH_COMPONENT;
            *internalFormat = GL_DEPTH_COMPONENT32F;
            *type = GL_FLOAT;
            break;
        default:
            assert(false);
    }
}

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
#ifdef DSA
    glCreateTextures(GL_TEXTURE_2D, 1, &texture.handle.value);
#else
    glGenTextures(1, &texture.handle.value);
    glBindTexture(GL_TEXTURE_2D, texture.handle.value);
#endif
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum formatType, internalFormat, type;
    TextureFormat2GL(texture.format, &formatType, &internalFormat, &type);

    glTextureStorage2D(texture.handle.value, 1, internalFormat, texture.width, texture.height);
    glTextureSubImage2D(texture.handle.value, 0, 0, 0, width, height, formatType, type, texture.data);

    switch (texture.scaling) {
        case MipmapScaling::BILINEAR:
            glTextureParameterf(texture.handle.value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameterf(texture.handle.value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case MipmapScaling::NEAREST_NEIGHBOUR:
            assert(false && "Not implemented nearest neighbour filtering");
            //TODO: get nearest neighbour scaling done for pixel 3D
            glTextureParameterf(texture.handle.value, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameterf(texture.handle.value, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        default:
            // assert(false && "No scaling set");
            break;
    }

    switch (texture.wrap) {
        case TextureWrapping::REPEAT:
            glTextureParameteri(texture.handle.value, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(texture.handle.value, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case TextureWrapping::CLAMP:
            glTextureParameteri(texture.handle.value, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(texture.handle.value, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        default:
            break;
    }

    if (texture.generateMipMaps) {
            glGenerateTextureMipmap(texture.handle.value);
    }

    // glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

inline RenderTextureArray CreateTexture2DArray(s32 count, RTextureFormat format, u32 width, u32 height, MipmapScaling scaling, TextureWrapping wrap) {
    RenderTextureArray texture{"TextureArray", count, format, width, height, false, scaling, wrap};
    // glGenTextures(1, &texture.handle.value);
    // glBindTexture(GL_TEXTURE_2D_ARRAY, texture.handle.value);
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture.handle.value);
    GLenum glformat;
    GLenum gltype;
    switch(format) {
        case RGBA8:
            // glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
                // width, height, count, 0, GL_RGBA8, GL_UNSIGNED_BYTE, nullptr);
            glformat = GL_RGBA8;
            gltype = GL_UNSIGNED_BYTE;
            break;
        case DEPTH16F:
            // glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16,
                // width, height, count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glformat = GL_DEPTH_COMPONENT16;
            gltype = GL_FLOAT;
            break;
        case DEPTH32F:
            // glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
            //     width, height, count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glformat = GL_DEPTH_COMPONENT32F;
            gltype = GL_FLOAT;
            break;
        default:
            assert(false && "Not supported format for texture arrays");
            break;
    }
    glTextureStorage3D(texture.handle.value, 0, glformat, width, height, count);
    glTextureSubImage3D(texture.handle.value, 0, 0, 0, 0, width, height, count, glformat, gltype, nullptr);
    switch(scaling) {
        case BILINEAR:
            glTextureParameteri(texture.handle.value, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(texture.handle.value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        default: 
            assert(false && "Not supported scaling for texture arrays");
            break;
    }
    switch(wrap) {
        case BORDER_COLOUR: {
            glTextureParameteri(texture.handle.value, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(texture.handle.value, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            f32 borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTextureParameterfv(texture.handle.value, GL_TEXTURE_BORDER_COLOR, borderColor);
            break;
        }
        default:
            assert(false && "Not supported wrapping method for texture arrays");
            break;
    }
    // Do they always have a compare mode?
    glTextureParameteri(texture.handle.value, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTextureParameteri(texture.handle.value, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    return texture;
}

RenderBuffer CreateBuffer(BufferType type, s32 vertexCount, s32 sizeElement, u8* data, DrawState state) {
    RenderBuffer buffer{vertexCount, sizeElement, data, state, type};
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
    glCreateBuffers(1, &buffer.handle.value);
    glBufferData(bufferType, vertexCount * sizeElement, data, glDrawType);
    glNamedBufferStorage(buffer.handle.value, vertexCount * sizeElement, data, glDrawState(state));

    return buffer;
}

void GenerateMipmap(RenderTexture2D& texture) {
    glGenerateTextureMipmap(texture.handle.value);
}

void GenerateMipmap(RenderTextureArray& texture) {
    glGenerateTextureMipmap(texture.handle.value);
}

void GenerateMipmap(TextureCubemap& texture) {
    glGenerateTextureMipmap(texture.handle.value);
}

void DeleteFramebuffer(Framebuffer& framebuffer) {
    glDeleteFramebuffers(1, &framebuffer.handle.value);
};

void BindFramebuffer(Framebuffer& framebuffer) {

}

void SetRenderState(RenderState& pipeline) {
}

VertexArray CreateVertexArray() {
    VertexArray array{};
    glCreateVertexArrays(1, &array.handle.value);
    return array;
}

void SetVertexArrayFormat(VertexArray& array, s32 numberOfElements, VertexArrayElement* elements) {
    u32 stride = 0;
    for (s32 i = 0; i < numberOfElements; i++) {
        stride += elements[i].numberOfElements;
    }
    u32 offset  = 0;
    for (s32 i = 0; i < numberOfElements; i++) {
        glEnableVertexArrayAttrib(array.handle.value, i);
        glVertexArrayAttribFormat(array.handle.value, i, elements[i].numberOfElements, GL_FLOAT, GL_FALSE, offset);
        glVertexArrayAttribBinding(array.handle.value, i, 0);
        offset += elements[i].numberOfElements * sizeof(f32);
    }
    array.stride = stride;
};

void SetVertexArrayVertexBuffer(VertexArray& array, RenderBuffer& vertexBuffer) {
    glVertexArrayVertexBuffer(array.handle.value, 0, vertexBuffer.handle.value, 0, array.stride);
}
void SetVertexArrayIndexBuffer(VertexArray& array, RenderBuffer& indexBuffer) {
    glVertexArrayElementBuffer(array.handle.value, indexBuffer.handle.value);
}

Framebuffer CreateFramebuffer() {
    Framebuffer framebuffer;
    glCreateFramebuffers(1, &framebuffer.handle.value);
    return framebuffer;
}

void FramebufferAttachColor(Framebuffer& framebuffer, RenderResource& colorAttachment, u32 slot) {
    u32 handle;
    switch (colorAttachment.type) {
    case RESOURCE_TEXTURE2D:
        handle = colorAttachment.texture2D.handle.value;
        break;
    case RESOURCE_TEXTURE2D_ARRAY:
        handle = colorAttachment.textureArray.handle.value;
        break;
    case RESOURCE_CUBEMAP:
        handle = colorAttachment.cubemap.handle.value;
        break;
    case RESOURCE_BUFFER:
        assert(false && "IDK what this is meant to do?");
        break;
    case RESOURCE_NONE:
        assert(false && "Trying to attach color when there is none!");
        break;
    }

    glNamedFramebufferTexture(framebuffer.handle.value, GL_COLOR_ATTACHMENT0+slot, handle, 0);
}

void FramebufferAttachDepth(Framebuffer& framebuffer, RenderResource& depthAttachment) {
    u32 handle;
    switch (depthAttachment.type) {
    case RESOURCE_TEXTURE2D:
        handle = depthAttachment.texture2D.handle.value;
        break;
    case RESOURCE_TEXTURE2D_ARRAY:
        handle = depthAttachment.textureArray.handle.value;
        break;
    case RESOURCE_CUBEMAP:
        handle = depthAttachment.cubemap.handle.value;
        break;
    case RESOURCE_BUFFER:
        assert(false && "IDK what this is meant to do?");
        break;
    case RESOURCE_NONE:
        assert(false && "Trying to attach color when there is none!");
        break;
    }

    glNamedFramebufferTexture(framebuffer.handle.value, GL_DEPTH_COMPONENT, handle, 0);
}


void FramebufferDrawAttachments(Framebuffer& framebuffer, u32 size) {
    GLenum drawBuffers[8];
    for (GLuint i = 0; i < size; i++) {
        drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
    }
    glNamedFramebufferDrawBuffers(framebuffer.handle.value, size, drawBuffers);
}

void BindFramebuffer(Framebuffer &framebuffer, u32 width, u32 height) {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.handle.value);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

RenderBufferHandle CreateRenderbuffer(Framebuffer& framebuffer, u32 width, u32 height, DepthComp depthComp) {
    RenderBufferHandle handle;

    glCreateRenderbuffers(1, &handle.value);
    GLenum glDepthComp;
    switch(depthComp) {
        case DepthComp::DepthComponent:
            glDepthComp = GL_DEPTH_COMPONENT;
            break;
        case DepthComp::DepthComponent24:
            glDepthComp = GL_DEPTH_COMPONENT24;
            break;
    }
    glNamedRenderbufferStorage(handle.value, glDepthComp, width, height);
    glNamedFramebufferRenderbuffer(framebuffer.handle.value, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, handle.value);
    return handle;
}

void FramebufferNoDrawBuffer(Framebuffer& framebuffer) {
    glNamedFramebufferDrawBuffer(framebuffer.handle.value, GL_NONE);
}

void FrameBufferNoReadBuffer(Framebuffer& framebuffer) {
    glNamedFramebufferReadBuffer(framebuffer.handle.value, GL_NONE);
}

TextureCubemap CreateTextureCubemap(u32 width, u32 height, RTextureFormat format, MipmapScaling scaling, TextureWrapping wrap) {
    TextureCubemap cubemap;
    cubemap.width = width;
    cubemap.height = height;
    cubemap.format = format;
    cubemap.scaling = scaling;
    cubemap.wrap = wrap;

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap.handle.value);

    GLenum glInternalFormat;
    GLenum glFormat;
    GLenum glType;
    switch (format) {
        case RTextureFormat::DEPTH16F:
            glFormat = GL_DEPTH_COMPONENT;
            glInternalFormat = GL_DEPTH_COMPONENT;
            glType = GL_FLOAT;
            break;
        case RTextureFormat::RGB16F:
            glFormat = GL_RGB;
            glInternalFormat = GL_RGB16F;
            glType = GL_FLOAT;
        default:
            assert(false);
    }

    glTextureStorage2D(cubemap.handle.value, 1, glInternalFormat, cubemap.width, cubemap.height);
    for (s32 i = 0; i < 6; ++i) {
        glTextureSubImage3D(cubemap.handle.value, 0, 0, 0, i, cubemap.width, cubemap.height, 1, glFormat, glType, NULL);
    }

    //TODO add options to change
    glTextureParameteri(cubemap.handle.value, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(cubemap.handle.value, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(cubemap.handle.value, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubemap.handle.value, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubemap.handle.value, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return cubemap;
}


UniformBuffer CreateUniformBuffer(u32 size, DrawState state) {
    UniformBuffer result;
    glCreateBuffers(1, &result.handle);
    glNamedBufferData(result.handle, size, nullptr, glDrawState(state));
}

void SetUniformSlot(UniformBuffer& uniform, u32 slot) {
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, uniform.handle);
}

void SetRenderClearColor(f32 r, f32 g, f32 b, f32 a) {
    glClearColor(r, g, b, a);
}

GLenum ToGLDepthFunc(DepthFunc func)
{
    switch(func)
    {
        case DEPTH_LESS:
            return GL_LESS;

        case DEPTH_LESS_EQUAL:
            return GL_LEQUAL;

        case DEPTH_GREATER:
            return GL_GREATER;

        case DEPTH_ALWAYS:
            return GL_ALWAYS;

        default:
            return GL_LESS;
    }
}


GLenum ToGLBlendFactor(BlendFactor factor)
{
    switch(factor)
    {
        case BLEND_ZERO:
            return GL_ZERO;

        case BLEND_ONE:
            return GL_ONE;

        case BLEND_SRC_ALPHA:
            return GL_SRC_ALPHA;

        case BLEND_ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
    }

    return GL_ONE;
}

void SetDepthState(const RenderState& state)
{
    if(state.depthTesting)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(ToGLDepthFunc(state.depthFunction));
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(state.depthWriting ? GL_TRUE : GL_FALSE);
}

void SetBlendState(const RenderState& state)
{
    if(state.blending) {
        glEnable(GL_BLEND);

        glBlendFunc(
            ToGLBlendFactor(state.srcBlend),
            ToGLBlendFactor(state.dstBlend)
        );
    } else {
        glDisable(GL_BLEND);
    }
}

void SetCullState(const RenderState& state)
{
    if(state.cullFace) {
        glEnable(GL_CULL_FACE);

        if(state.cullFaceMode == CULL_BACK)
            glCullFace(GL_BACK);
        else
            glCullFace(GL_FRONT);


        if(state.frontFace == FRONT_CCW)
            glFrontFace(GL_CCW);
        else
            glFrontFace(GL_CW);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void SetPolygonState(const RenderState& state)
{
    if(state.polygonMode == POLYGON_FILL)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SetTextureCubeMapSeamlessState(const RenderState& state) {
    if (state.textureCubeMapSeamless) {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    } else {
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }
}

std::string read_file_to_string(const char* path)
{
    std::string s = "";
    std::ifstream s_stream(path, std::ios::in);
    if (!s_stream.is_open())
    {
        std::cout << "Could not read/load file " << path << ". " << std::endl;
        assert(true);
    }

    std::string line = "";
    while (!s_stream.eof())
    {
        std::getline(s_stream, line);
        s.append(line + "\n");
    }

    s_stream.close();
    return s;
}

ShaderHandle ShaderCompile(const char* vs, const char* fs, const char* gs) {
    
    // Create an empty vertex shader handle
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    std::string vs_src = read_file_to_string(vs);
    std::string fs_src = read_file_to_string(fs);

    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const GLchar* source = vs_src.c_str();
    glShaderSource(vertex_shader, 1, &source, 0);

    // Compile the vertex shader
    glCompileShader(vertex_shader);

    GLint is_compiled = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(vertex_shader);

        // Use the infoLog as you see fit.
        std::cout << "Vertex shader compilation failure!" << std::endl;
        std::cout << vs << std::endl;
        std::cout << (const char*)infoLog.data() << std::endl;

        return {0};
    }

    // Create an empty fragment shader handle
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    source = (const GLchar*)fs_src.c_str();
    glShaderSource(fragment_shader, 1, &source, 0);

    // Compile the fragment shader
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(fragment_shader);
        // Either of them. Don't leak shaders.
        glDeleteShader(vertex_shader);

        // Use the infoLog as you see fit.
        std::cout << "Fragment shader compilation failure!" << std::endl;
        std::cout << fs << std::endl;
        std::cout << (const char*)infoLog.data() << std::endl;

        // In this simple program, we'll just leave
        return {0};
    }


    GLuint geometry_shader = 0;
    if (gs)
    {
        std::string gs_src = read_file_to_string(gs);
        // Create an empty fragment shader handle
        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);

        // Send the fragment shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        source = (const GLchar*)gs_src.c_str();
        glShaderSource(geometry_shader, 1, &source, 0);

        // Compile the fragment shader
        glCompileShader(geometry_shader);

        glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &is_compiled);
        if (is_compiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(geometry_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(geometry_shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(fragment_shader);
            // Either of them. Don't leak shaders.
            glDeleteShader(vertex_shader);

            // Use the infoLog as you see fit.
            std::cout << "Geometry shader compilation failure!" << std::endl;
            std::cout << gs << std::endl;
            std::cout << (const char*)infoLog.data() << std::endl;

            // In this simple program, we'll just leave
            return {0};
        }
    }
    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    GLuint program = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    if (gs)
    {
        glAttachShader(program, geometry_shader);
    }

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        // Use the infoLog as you see fit.
        std::cout << "Shader link failure!" << std::endl;
        std::cout << (const char*)infoLog.data() << std::endl;

        // In this simple program, we'll just leave
        return {0};
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    if (gs) glDetachShader(program, geometry_shader);

    return { program };
}

void DeleteShader(ShaderHandle shader) {
    glDeleteProgram(shader.handle);
}

void BindShader(ShaderHandle shader) {
    glUseProgram(shader.handle);
}

void ShaderUnbind() {
    glUseProgram(0);
}

s32 ShaderUniformLocation(ShaderHandle shader, const char* name)
{
    s32 location = glGetUniformLocation(shader.handle, name);
    if (location == -1)
    {
        std::cout << "Uniform " << name << " not found for shader " << "" << std::endl;
        assert(false);
    }
    return location;
}


void ShaderUniformFloat(ShaderHandle shader, const char* name, const f32 value) {
    glUniform1f(ShaderUniformLocation(shader, name), value);
}

void ShaderUniformInt(ShaderHandle shader, const char* name, const s32 value) {
    glUniform1i(ShaderUniformLocation(shader, name), value);
}

void ShaderUniformVector2f(ShaderHandle shader, const char* name, const vec2& value) {
    glUniform2f(ShaderUniformLocation(shader, name), value.x, value.y);
}

void ShaderUniformVector3f(ShaderHandle shader, const char* name, const vec3& value) {
    glUniform3f(ShaderUniformLocation(shader, name), value.x, value.y, value.z);
}
void ShaderUniformVector4f(ShaderHandle shader, const char* name, const vec4& value) {
    glUniform4f(ShaderUniformLocation(shader, name), value.x, value.y, value.z, value.w);
}

void ShaderUniformMatrix4(ShaderHandle shader, const char* name, const mat4& value) {
    glUniformMatrix4fv(ShaderUniformLocation(shader, name), 1, GL_FALSE, glm::value_ptr(value));
}



