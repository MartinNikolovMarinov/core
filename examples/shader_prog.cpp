#include "shader_prog.h"

namespace
{

core::expected<u32, ShaderProg::error_type> compile_shader(u32 type, std::string_view src) {
    u32 shader = glCreateShader(type);
    if (shader == 0) return core::unexpected("Failed to create shader");
    const char* srcPtr = src.data();
    glShaderSource(shader, 1, &srcPtr, nullptr);
    glCompileShader(shader);

    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        return core::unexpected(fmt::format("Failed to compile shader: {}", infoLog));
    }

    return shader;
}

} // namespace

core::expected<ShaderProg, ShaderProg::error_type> ShaderProg::create(std::string_view vertexShaderSrc,
                                                                      std::string_view fragShaderSrc) {
    if (vertexShaderSrc.empty()) return core::unexpected("Vertex shader source is empty.");
    if (fragShaderSrc.empty())   return core::unexpected("Fragment shader source is empty.");

    u32 prog = glCreateProgram();

    auto res = compile_shader(GL_VERTEX_SHADER, vertexShaderSrc);
    if (res.has_err()) return core::unexpected(std::move(res.err()));
    u32 vs = res.value();
    defer { glDeleteShader(vs); };

    auto res2 = compile_shader(GL_FRAGMENT_SHADER, fragShaderSrc);
    if (res2.has_err()) return core::unexpected(std::move(res2.err()));
    u32 fs = res2.value();
    defer { glDeleteShader(fs); };

    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glValidateProgram(prog);

    i32 success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        return core::unexpected(fmt::format("Failed to create shader program: {}", infoLog));
    }

    ShaderProg ret(prog, vs, fs);
    return ret;
}

