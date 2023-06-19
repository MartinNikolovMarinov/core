#include "box.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void Box::translate(const core::vec3f& translation) {
    this->transform = core::translate(transform, translation);
}

void Box::scale(const core::vec3f& scale) {
    this->transform = core::scale(transform, scale);
}

void Box::rotate(const core::vec3f& axis, const core::radians& angle) {
    this->transform = core::rotate(transform, axis, angle);
}

void Box::setColor(const core::vec4f& color) {
    this->color = color;
}


namespace {

constexpr i32 CUBE_VERTICES_COUNT = 36;
constexpr core::vec3f g_cubeVertices[CUBE_VERTICES_COUNT] = {
    core::v(-0.5f, -0.5f, -0.5f),
    core::v(0.5f, -0.5f, -0.5f),
    core::v(0.5f,  0.5f, -0.5f),
    core::v(0.5f,  0.5f, -0.5f),
    core::v(-0.5f,  0.5f, -0.5f),
    core::v(-0.5f, -0.5f, -0.5f),

    core::v(-0.5f, -0.5f,  0.5f),
    core::v(0.5f, -0.5f,  0.5f),
    core::v(0.5f,  0.5f,  0.5f),
    core::v(0.5f,  0.5f,  0.5f),
    core::v(-0.5f,  0.5f,  0.5f),
    core::v(-0.5f, -0.5f,  0.5f),

    core::v(-0.5f,  0.5f,  0.5f),
    core::v(-0.5f,  0.5f, -0.5f),
    core::v(-0.5f, -0.5f, -0.5f),
    core::v(-0.5f, -0.5f, -0.5f),
    core::v(-0.5f, -0.5f,  0.5f),
    core::v(-0.5f,  0.5f,  0.5f),

    core::v(0.5f,  0.5f,  0.5f),
    core::v(0.5f,  0.5f, -0.5f),
    core::v(0.5f, -0.5f, -0.5f),
    core::v(0.5f, -0.5f, -0.5f),
    core::v(0.5f, -0.5f,  0.5f),
    core::v(0.5f,  0.5f,  0.5f),

    core::v(-0.5f, -0.5f, -0.5f),
    core::v(0.5f, -0.5f, -0.5f),
    core::v(0.5f, -0.5f,  0.5f),
    core::v(0.5f, -0.5f,  0.5f),
    core::v(-0.5f, -0.5f,  0.5f),
    core::v(-0.5f, -0.5f, -0.5f),

    core::v(-0.5f,  0.5f, -0.5f),
    core::v(0.5f,  0.5f, -0.5f),
    core::v(0.5f,  0.5f,  0.5f),
    core::v(0.5f,  0.5f,  0.5f),
    core::v(-0.5f,  0.5f,  0.5f),
    core::v(-0.5f,  0.5f, -0.5f),
};

u32 g_cubeVBO;
u32 g_cubeVAO;

} // namespace

void immediate_cubeRendererInit(u32 inPosAttribLocation) {
    glGenBuffers(1, &g_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_cubeVBO);
    ptr_size bufferDataSize = sizeof(core::vec3f) * CUBE_VERTICES_COUNT;
    glBufferData(GL_ARRAY_BUFFER, bufferDataSize, g_cubeVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &g_cubeVAO);
    glBindVertexArray(g_cubeVAO);

    constexpr ptr_size stride = sizeof(core::vec3f);
    constexpr ptr_size dimensions = core::vec3f::dimensions();
    glVertexAttribPointer(inPosAttribLocation, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(inPosAttribLocation);
}

void immediate_cubeRendererDestroy() {
    glDeleteBuffers(1, &g_cubeVBO);
    glDeleteVertexArrays(1, &g_cubeVAO);
}

void immediate_render(ShaderProg& prog, const Box& cube) {
    prog.setUniform_color("color", cube.color);
    prog.setUniform_model("u_model", cube.transform);

    glBindVertexArray(g_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_cubeVBO);
    glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTICES_COUNT);
}
