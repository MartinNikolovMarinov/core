#include "mesh.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

u32 Mesh2D::Usage::usage_Gl() const {
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

Mesh2D Mesh2D::create(const Mesh2D::VertexLayout& vl, core::arr<core::vec2f>&& vertices) {
    Mesh2D m;
    m.m_vertices = core::move(vertices);
    m.m_vboId = -1;
    m.m_vaoId = -1;

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
    constexpr ptr_size dimmentions = core::vec2f::dimmentions();
    glVertexAttribPointer(vl.posAttribId, dimmentions, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(vl.posAttribId);

    return m;
}

u32 Mesh2D::vbo_id() const { return m_vboId; }
u32 Mesh2D::vao_id() const { return m_vaoId; }
const core::arr<core::vec2f>& Mesh2D::vertices() const { return m_vertices; }
i32 Mesh2D::vertex_count() const { return m_vertices.len(); }

void Mesh2D::destroy() const {
    glDeleteBuffers(1, &m_vboId);
    glDeleteVertexArrays(1, &m_vaoId);
}

