#include "shape.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

u32 Shape2D::Usage::usage_Gl() const {
    u32 ret = -1;
    if (a == Access::STATIC) {
        if (t == AccessType::DRAW) ret = GL_STATIC_DRAW;
        if (t == AccessType::READ) ret = GL_STATIC_READ;
        if (t == AccessType::COPY) ret = GL_STATIC_COPY;
    }
    if (a == Access::DYNAMIC) {
        if (t == AccessType::DRAW) ret = GL_DYNAMIC_DRAW;
        if (t == AccessType::READ) ret = GL_DYNAMIC_READ;
        if (t == AccessType::COPY) ret = GL_DYNAMIC_COPY;
    }
    if (a == Access::STREAM) {
        if (t == AccessType::DRAW) ret = GL_STREAM_DRAW;
        if (t == AccessType::READ) ret = GL_STREAM_READ;
        if (t == AccessType::COPY) ret = GL_STREAM_COPY;
    }
    return ret;
}

Shape2D Shape2D::create(const Shape2D::VertexLayout& vl,
                        const core::vec4f& fillColor,
                        f32 zIndex,
                        core::arr<core::vec2f>&& vertices) {
    Shape2D m;
    m.m_vertices = core::move(vertices);
    m.m_vboId = -1;
    m.m_vaoId = -1;
    m.m_zIndex = zIndex;
    m.m_fillColor = fillColor;

    // Create VBO
    glGenBuffers(1, &m.m_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, m.m_vboId);
    ptr_size bufferDataSize = m.m_vertices.len() * sizeof(core::vec2f);
    glBufferData(GL_ARRAY_BUFFER, bufferDataSize, m.m_vertices.data(), vl.usage.usage_Gl());

    // Create VAO
    glGenVertexArrays(1, &m.m_vaoId);
    glBindVertexArray(m.m_vaoId);

    // Link vertex attributes
    constexpr ptr_size stride = sizeof(core::vec2f);
    constexpr ptr_size dimentions = core::vec2f::dimentions();
    glVertexAttribPointer(vl.posAttribId, dimentions, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(vl.posAttribId);

    return m;
}

u32 Shape2D::vbo_id() const { return m_vboId; }
u32 Shape2D::vao_id() const { return m_vaoId; }
f32 Shape2D::z_index() const { return m_zIndex; }
const core::arr<core::vec2f>& Shape2D::vertices() const { return m_vertices; }
i32 Shape2D::vertex_count() const { return m_vertices.len(); }
const core::vec4f& Shape2D::fill_color() const { return m_fillColor; }

void Shape2D::destroy() const {
    glDeleteBuffers(1, &m_vboId);
    glDeleteVertexArrays(1, &m_vaoId);
}

