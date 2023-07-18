#pragma once

#include "init_core.h"

#include <GL/glew.h>
#include <fmt/format.h>

#include <string>
#include <string_view>

using namespace coretypes;

struct ShaderProg {
    // TODO2: using a string as the error type is a bit crazy, but it's for this stage.
    using error_type = std::string;

    static core::expected<ShaderProg, error_type> create(std::string_view vertexShaderSrc,
                                                         std::string_view fragShaderSrc);

    ShaderProg() = default;

    u32 progId()          const { return m_id; }
    u32 vertexShaderId() const { return m_vertexShaderId; }
    u32 fragShaderId()   const { return m_fragShaderId; }

    void destroy() { glDeleteProgram(m_id); }
    void use() { glUseProgram(m_id); }

    core::expected<error_type> setUniform_color(std::string_view name, const core::vec4f& color);
    core::expected<error_type> setUniform_model(std::string_view name, const core::mat4f& model);
    core::expected<error_type> setUniform_view(std::string_view name, const core::mat4f& view);
    core::expected<error_type> setUniform_proj(std::string_view name, const core::mat4f& proj);

    template <i32 Dim, typename T>
    core::expected<error_type> setUniform_v(std::string_view name, const core::vec<Dim, T>& v) {
        i32 loc = glGetUniformLocation(m_id, name.data());
        if (loc < 0) return core::unexpected(fmt::format("Failed to get uniform location for {}", name));
        if constexpr (std::is_same_v<T, f32>) {
            if      constexpr (Dim == 2) glUniform2fv(loc, 1, reinterpret_cast<const f32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3fv(loc, 1, reinterpret_cast<const f32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4fv(loc, 1, reinterpret_cast<const f32*>(&v.data[0]));
            else    static_assert(core::always_false<T>, "Unsupported type");
        }
        else if constexpr (std::is_same_v<T, i32>) {
            if      constexpr (Dim == 2) glUniform2iv(loc, 1, reinterpret_cast<const i32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3iv(loc, 1, reinterpret_cast<const i32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4iv(loc, 1, reinterpret_cast<const i32*>(&v.data[0]));
            else    static_assert(core::always_false<T>, "Unsupported type");
        }
        else if constexpr (std::is_same_v<T, u32>) {
            if      constexpr (Dim == 2) glUniform2uiv(loc, 1, reinterpret_cast<const u32*>(&v.data[0]));
            else if constexpr (Dim == 3) glUniform3uiv(loc, 1, reinterpret_cast<const u32*>(&v.data[0]));
            else if constexpr (Dim == 4) glUniform4uiv(loc, 1, reinterpret_cast<const u32*>(&v.data[0]));
            else    static_assert(core::always_false<T>, "Unsupported type");
        }
        else {
            static_assert(core::always_false<T>, "Unsupported type");
        }
        return {};
    }

    template <i32 TRow, i32 TCol, typename T>
    core::expected<error_type> setUniform_m(std::string_view name, const core::mat<TRow, TCol, T>& m) {
        i32 loc = glGetUniformLocation(m_id, name.data());
        if (loc < 0) return core::unexpected(fmt::format("Failed to get uniform location for {}", name));
        if constexpr (std::is_same_v<T, f32>) {
            if      constexpr (TRow == 2 && TCol == 2) glUniformMatrix2fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 3 && TCol == 3) glUniformMatrix3fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 4 && TCol == 4) glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 2 && TCol == 3) glUniformMatrix2x3fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 3 && TCol == 2) glUniformMatrix3x2fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 2 && TCol == 4) glUniformMatrix2x4fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 4 && TCol == 2) glUniformMatrix4x2fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 3 && TCol == 4) glUniformMatrix3x4fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else if constexpr (TRow == 4 && TCol == 3) glUniformMatrix4x3fv(loc, 1, GL_FALSE, reinterpret_cast<const f32*>(&m.data[0]));
            else    static_assert(core::always_false<T>, "Unsupported type");
        }
        else {
            static_assert(core::always_false<T>, "Unsupported type");
        }
        return {};
    }

    template <i32 Dim, typename T>
    core::expected<error_type> setUniform_v(std::string_view name, const core::vec<Dim, T>&& v) {
        return setUniform_v(name, v);
    }

    template <typename T>
    core::expected<error_type> setUniform(std::string_view name, T val) {
        i32 loc = glGetUniformLocation(m_id, name.data());
        if (loc < 0) return core::unexpected(fmt::format("Failed to get uniform location for {}", name));
        if constexpr      (std::is_same_v<T, f32>) glUniform1f(loc, val);
        else if constexpr (std::is_same_v<T, i32>) glUniform1i(loc, val);
        else if constexpr (std::is_same_v<T, u32>) glUniform1ui(loc, val);
        else    static_assert(core::always_false<T>, "Unsupported type");
        return {};
    }

    core::expected<i32, error_type> getAttribLocation(std::string_view name);

private:
    u32 m_id;
    u32 m_vertexShaderId;
    u32 m_fragShaderId;

    ShaderProg(u32 id, u32 vertexShaderId, u32 fragShaderId)
        : m_id(id)
        , m_vertexShaderId(vertexShaderId)
        , m_fragShaderId(fragShaderId) {};
};
