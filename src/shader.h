#ifndef SHADER_HH
#define SHADER_HH

class Shader
{
public:
    Shader(const char* filepath);
    ~Shader();

    u32
    program() { return program_; }

    void
    set_program(u32 program) { program_ = program; }

private:
    u32 program_;
};  

#endif