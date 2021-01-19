#include "anthology_opengl.h"

#include <string>
#include <vector>
#include <fstream>

#include <glad/glad.h>

void bind_index_buffer(IndexBuffer* index_buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id);
}

void create_index_buffer(IndexBuffer* index_buffer, uint32* indices, uint32 count)
{
    glCreateBuffers(1, &index_buffer->id);
    bind_index_buffer(index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, indices, GL_STATIC_DRAW);
}

void unbind_index_buffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void delete_index_buffer(IndexBuffer* index_buffer)
{
    glDeleteBuffers(1, &index_buffer->id);
}

void bind_vertex_buffer(VertexBuffer* vertex_buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id);
}

void create_vertex_buffer(VertexBuffer* vertex_buffer, float* vertices, uint32 size)
{
    glCreateBuffers(1, &vertex_buffer->id);
    bind_vertex_buffer(vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void unbind_vertex_buffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void delete_vertex_buffer(VertexBuffer* vertex_buffer)
{
    glDeleteBuffers(1, &vertex_buffer->id);
}

void bind_vertex_array(VertexArray* vertex_array)
{
    glBindVertexArray(vertex_array->id);
}

void delete_vertex_array(VertexArray* vertex_array)
{
    glDeleteVertexArrays(1, &vertex_array->id);
}

void unbind_vertex_array(VertexArray* vertex_array)
{
    glBindVertexArray(0);
}

uint32 shader_type_size(ShaderType type)
{
    switch(type)
    {
        case SHADER_FLOAT:
            return sizeof(float);
        default:
            return 0;
    }
}

GLenum convert_shadertype_to_gltype(ShaderType type)
{
    switch(type)
    {
        case ShaderType::SHADER_FLOAT: return GL_FLOAT;
        default: return 0;
    }
}

void assign_vertex_attrib_layout(BufferLayoutAttrib* layout, uint32 number_of_attrib)
{
    uint32 last_data_stride = 0;
    uint32 number_of_elements = 0;
    for (uint32 index = 0; index < number_of_attrib; ++index)
    {
        number_of_elements += layout[index].count;
    }

    for (uint32 index = 0; index < number_of_attrib; ++index)
    {
        glVertexAttribPointer(index,
                              layout[index].count,
                              convert_shadertype_to_gltype(layout[index].type),
                              GL_FALSE,
                              number_of_elements * shader_type_size(layout[index].type),
                              (void*)(last_data_stride));
        glEnableVertexAttribArray(index);
        last_data_stride += layout[index].count * shader_type_size(layout[index].type);
    }
}

void create_vertex_array(VertexArray* vertex_array)
{
    glGenVertexArrays(1, &vertex_array->id);
    bind_vertex_array(vertex_array);
}

void bind_shader(Shader* shader)
{
    glUseProgram(shader->program);
}

void unbind_shader(Shader* shader)
{
    glUseProgram(0);
}

void delete_shader(Shader* shader)
{
    glDeleteProgram(shader->program);
}

void compile_shader(Shader* shader, const char* fragment_src, const char* vertex_src)
{
    // Create an empty vertex shader handle
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const GLchar* source = vertex_src;
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
        printf("Vertex shader compilation failure!");
        printf(infoLog.data());

        // In this simple program, we'll just leave
        return;
    }

    // Create an empty fragment shader handle
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    source = (const GLchar*)fragment_src;
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
        printf("Fragment shader compilation failure!");
        printf(infoLog.data());

        // In this simple program, we'll just leave
        return;
    }

    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    GLuint program = glCreateProgram();

    shader->program = program;

    // Attach our shaders to our program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

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
        printf("Shader link failure!");
        printf(infoLog.data());

        // In this simple program, we'll just leave
        return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
}

void load_shader(Shader* shader, const char* file_path)
{
    std::ifstream stream(file_path);
    std::string line;

    std::string fragment_shader = "";
    std::string vertex_shader = "";
    bool is_fragment = false;
    
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                is_fragment = false;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                is_fragment = true;
            }
        }
        else
        {
            if (is_fragment)
                fragment_shader += line + "\n";
            else
                vertex_shader += line + "\n";
        }
    }

    compile_shader(shader, fragment_shader.c_str(), vertex_shader.c_str());
}

void upload_uniform_bool(const char* name, bool value, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    assert(location == -1);
    glUniform1i(location, (int32)value);
}

void upload_uniform_int32(const char* name, int32 value, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    if (location == -1)
        assert(true);

    glUniform1i(location, value);
}

void upload_uniform_float(const char* name, float value, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    //assert(location == -1);
    glUniform1f(location, value);
}

void upload_uniform_mat4(const char* name, glm::mat4 mat4, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    if (location == -1)
        printf("No such uniform known as %s", name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
}

void upload_uniform_vec3(const char* name, glm::vec3 vec3, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    if (location == -1)
        printf("Uniform not found");
    glUniform3fv(location, 1, glm::value_ptr(vec3));
}

void upload_uniform_vec4(const char* name, glm::vec4 vec4, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    if (location == -1)
        printf("Uniform not found");
    glUniform3fv(location, 1, glm::value_ptr(vec4));
}

void clear_screen()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLenum convert_to_gl_filter(TextureFilter filter)
{
    switch(filter)
    {
        case TextureFilter::NEAREST_NEIGHBOR:
            return GL_NEAREST;
        case TextureFilter::BILINEAR:
            return GL_LINEAR;
        default:
            return GL_LINEAR;
    }
}

GLenum convert_to_gl_wrap(TextureWrap wrap) 
{
    switch(wrap)
    {
        case TextureWrap::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        case TextureWrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case TextureWrap::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case TextureWrap::REPEAT:
            return GL_REPEAT;
        default:
            return GL_CLAMP_TO_EDGE;
    }
}

void create_texture(Texture2D* texture, LoadedTexture2D* loaded_texture)
{
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert_to_gl_filter(loaded_texture->filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert_to_gl_filter(loaded_texture->filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert_to_gl_wrap(loaded_texture->wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert_to_gl_wrap(loaded_texture->wrap));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loaded_texture->width, loaded_texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_texture->image_data);

    texture->loaded_texture = loaded_texture;
}

void bind_texture(Texture2D* texture, uint32 slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void unbind_texture(Texture2D* texture)
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void delete_texture(Texture2D* texture)
{
    glDeleteTextures(1, &texture->id);
}

void upload_material_uniform(Material* material, Shader* shader)
{
    upload_uniform_int32("material.diffuse", material->diffuse.slot, shader);
    upload_uniform_vec3("material.specular", material->specular , shader);
    upload_uniform_float("material.shininess", material->shininess , shader);
}

void upload_spot_light_uniform(SpotLight* light, Shader* shader)
{
    upload_uniform_vec3("u_SpotLight.diffuse", light->diffuse, shader);
    upload_uniform_vec3("u_SpotLight.ambient", light->ambient, shader);
    upload_uniform_vec3("u_SpotLight.specular",  light->specular, shader);
    upload_uniform_vec3("u_SpotLight.position", light->position, shader);
    upload_uniform_vec3("u_SpotLight.direction", light->direction, shader);
    upload_uniform_float("u_SpotLight.innerCutOff", light->innerCutOff, shader);
    upload_uniform_float("u_SpotLight.outerCutOff", light->outerCutOff, shader);
}

void upload_directional_light_uniform(DirectionalLight* light, Shader* shader)
{
    upload_uniform_vec3("u_DirLight.diffuse", light->diffuse, shader);
    upload_uniform_vec3("u_DirLight.ambient", light->ambient, shader);
    upload_uniform_vec3("u_DirLight.specular",  light->specular, shader);
    upload_uniform_vec3("u_DirLight.direction", light->direction, shader);
}

void upload_point_light_uniform(PointLight* light, int32 index, Shader* shader)
{
    // TODO string concatentation is expensive maybe change?

    std::string name = "u_PointLights[" + std::to_string(index) + "]";

    std::string u_name = name + ".diffuse";
    upload_uniform_vec3(u_name.c_str(), light->diffuse, shader);
    u_name = name + ".ambient";
    upload_uniform_vec3(u_name.c_str(), light->ambient, shader);
    u_name = (name + ".specular");
    upload_uniform_vec3(u_name.c_str(),  light->specular, shader);
    u_name = (name + ".position");
    upload_uniform_vec3(u_name.c_str(), light->position, shader);
    u_name = (name + ".innerCutOff");
    upload_uniform_float(u_name.c_str(), light->constant, shader);
    u_name = (name + ".outerCutOff");
    upload_uniform_float(u_name.c_str(), light->linear, shader);
    u_name = (name + ".outerCutOff");
    upload_uniform_float(u_name.c_str(), light->quadratic, shader);
}