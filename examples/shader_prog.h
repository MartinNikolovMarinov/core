#pragma once

#include <GL/glew.h>

#include <core.h>
#include <std/core.h>

#include <string>
#include <string_view>

using namespace coretypes;

struct ShaderProg {
    using error_type = std::string;

    ShaderProg() = default;

    u32 prog_id()         const { return m_id; }
    u32 vertex_shader_id() const { return m_vertexShaderId; }
    u32 frag_shader_id()   const { return m_fragShaderId; }

    void destroy() { glDeleteProgram(m_id); }
    void use()     { glUseProgram(m_id); }

    template <i32 Dim, typename T>
    void uniform(std::string_view name, core::vec<Dim, T>& v) {
        i32 loc = glGetUniformLocation(m_id, name.data());
        Assert(loc >= 0, "Failed to get uniform location");
        if constexpr (std::is_same_v<T, f32>) {
            if      constexpr (Dim == 2) glUniform2fv(loc, 1, reinterpret_cast<f32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3fv(loc, 1, reinterpret_cast<f32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4fv(loc, 1, reinterpret_cast<f32*>(&v.data[0]));
        }
        else if constexpr (std::is_same_v<T, i32>) {
            if      constexpr (Dim == 2) glUniform2iv(loc, 1, reinterpret_cast<i32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3iv(loc, 1, reinterpret_cast<i32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4iv(loc, 1, reinterpret_cast<i32*>(&v.data[0]));
        }
        else if constexpr (std::is_same_v<T, u32>) {
            if      constexpr (Dim == 2) glUniform2uiv(loc, 1, reinterpret_cast<u32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3uiv(loc, 1, reinterpret_cast<u32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4uiv(loc, 1, reinterpret_cast<u32*>(&v.data[0]));
        }
        else {
            Assert(false, "Unsupported type");
        }
    }

    template <typename T>
    void uniform(std::string_view name, T val) {
        i32 loc = glGetUniformLocation(m_id, name.data());
        Assert(loc > 0, "Failed to get uniform location");
        if constexpr      (std::is_same_v<T, f32>) glUniform1f(loc, val);
        else if constexpr (std::is_same_v<T, i32>) glUniform1i(loc, val);
        else if constexpr (std::is_same_v<T, u32>) glUniform1ui(loc, val);
        else Assert(false, "Unsupported type");
    }

    static core::expected<ShaderProg, error_type> create(std::string_view vertexShaderSrc,
                                                         std::string_view fragShaderSrc);

private:
    u32 m_id;
    u32 m_vertexShaderId;
    u32 m_fragShaderId;

    ShaderProg(u32 id, u32 vertexShaderId, u32 fragShaderId)
        : m_id(id)
        , m_vertexShaderId(vertexShaderId)
        , m_fragShaderId(fragShaderId) {};
};
