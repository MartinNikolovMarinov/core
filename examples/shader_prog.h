#pragma once

#include "init_core.h"

#include <GL/glew.h>
#include <fmt/format.h>

#include <string>
#include <string_view>

using namespace coretypes;

struct ShaderProg {
    // TODO: using a string as the error type is a bit crazy, but it's ok for debugging.
    using error_type = std::string;

    static core::expected<ShaderProg, error_type> create(std::string_view vertexShaderSrc,
                                                         std::string_view fragShaderSrc);

    ShaderProg() = default;

    u32 prog_id()          const { return m_id; }
    u32 vertex_shader_id() const { return m_vertexShaderId; }
    u32 frag_shader_id()   const { return m_fragShaderId; }

    void destroy() { glDeleteProgram(m_id); }
    void use()     { glUseProgram(m_id); }

    template <i32 Dim, typename T>
    core::expected<error_type> set_uniform_v(std::string_view name, const core::vec<Dim, T>& v) {
        i32 loc = glGetUniformLocation(m_id, name.data());
        if (loc < 0) return core::unexpected(fmt::format("Failed to get uniform location for {}", name));
        if constexpr (std::is_same_v<T, f32>) {
            if      constexpr (Dim == 2) glUniform2fv(loc, 1, reinterpret_cast<const f32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3fv(loc, 1, reinterpret_cast<const f32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4fv(loc, 1, reinterpret_cast<const f32*>(&v.data[0]));
        }
        else if constexpr (std::is_same_v<T, i32>) {
            if      constexpr (Dim == 2) glUniform2iv(loc, 1, reinterpret_cast<const i32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3iv(loc, 1, reinterpret_cast<const i32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4iv(loc, 1, reinterpret_cast<const i32*>(&v.data[0]));
        }
        else if constexpr (std::is_same_v<T, u32>) {
            if      constexpr (Dim == 2) glUniform2uiv(loc, 1, reinterpret_cast<const u32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3uiv(loc, 1, reinterpret_cast<const u32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4uiv(loc, 1, reinterpret_cast<const u32*>(&v.data[0]));
        }
        else {
            static_assert(core::always_false<T>, "Unsupported type"); // honestly I hate c++ :D
        }
        return {};
    }

    template <i32 Dim, typename T>
    core::expected<error_type> set_uniform_v(std::string_view name, const core::vec<Dim, T>&& v) {
        return set_uniform_v(name, v);
    }

    template <typename T>
    core::expected<error_type> set_uniform(std::string_view name, T val) {
        i32 loc = glGetUniformLocation(m_id, name.data());
        if (loc < 0) return core::unexpected(fmt::format("Failed to get uniform location for {}", name));
        if constexpr      (std::is_same_v<T, f32>) glUniform1f(loc, val);
        else if constexpr (std::is_same_v<T, i32>) glUniform1i(loc, val);
        else if constexpr (std::is_same_v<T, u32>) glUniform1ui(loc, val);
        else    static_assert(core::always_false<T>, "Unsupported type");
        return {};
    }

    core::expected<i32, error_type> get_attrib_location(std::string_view name);

private:
    u32 m_id;
    u32 m_vertexShaderId;
    u32 m_fragShaderId;

    ShaderProg(u32 id, u32 vertexShaderId, u32 fragShaderId)
        : m_id(id)
        , m_vertexShaderId(vertexShaderId)
        , m_fragShaderId(fragShaderId) {};
};
