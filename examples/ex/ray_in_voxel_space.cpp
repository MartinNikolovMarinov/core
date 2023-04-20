#include "ray_in_voxel_space.h"

#include <keyboard.h>
#include <shader_prog.h>
#include <grid.h>

#include <std/stringer.h>

namespace ray_in_voxel_space {

namespace {

struct Ray2D {
    core::vec2f origin;
    core::vec2f direction;

    core::vec2f at(f32 t) const {
        return origin + t * direction;
    }
};

struct Cell {
    core::vec2f pos = {};
    bool isOn = false;
};

struct State {
    static constexpr Grid2D viewSpaceGrid  = { core::v(-1.0f, 1.0f), core::v(1.0f, -1.0f) };
    static constexpr Grid2D worldSpaceGrid = { core::v(0.0f, 0.0f), core::v(1000.0f, 1000.0f) };
    static constexpr core::mat4f worldSpaceToViewSpaceMatrix = worldSpaceGrid.getToConvMatrix(viewSpaceGrid);

    u32 viewportWidth;
    u32 viewportHeight;

    ShaderProg shaderProg;
    core::mat4f projectionMat;
    core::mat4f viewMat;

    u32 quadVAO = 0;
    u32 quadVBO = 0;
    u32 quadEBO = 0;
    u32 quadIndicesCount = 0;

    static constexpr u32 cellCount = 1600;
    static constexpr u32 cellWidth = 20;
    static constexpr u32 cellHeight = 20;
    static constexpr u32 cellsPerRow = 40;
    static constexpr core::vec2f gridOffset = worldSpaceGrid.center() - core::uniform<2, f32>(cellsPerRow * cellWidth / 2);
    Cell cells[cellCount] = {};

    u32 lineVAO = 0;
    u32 lineVBO = 0;
    Ray2D ray = {};
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

void resetOpenGLBinds() {
    // Debug reset binds to make sure everyting is bound correctly later on.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    State& g_s = state();
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

    glEnable(GL_DEPTH_TEST);

    g_s.viewportWidth = s.mainWindow.width;
    g_s.viewportHeight = s.mainWindow.height;

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        [[maybe_unused]] KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.isPressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW KeyCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow*, i32 width, i32 height) {
        State& g_s = state();
        glViewport(0, 0, width, height);
        g_s.viewportWidth = width;
        g_s.viewportHeight = height;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW FramebufferSizeCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    // Set Error callback:
    glfwSetErrorCallback([](i32 errCode, const char* errDesc) {
        fmt::print(stderr, "GLFW error: {}, reason: {}\n", errCode, errDesc ? errDesc : "Unknown");
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW Error callback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    return {};
}

void destroy() {
    State& g_s = state();
    glDeleteVertexArrays(1, &g_s.quadVAO);
    glDeleteBuffers(1, &g_s.quadVBO);
    glDeleteBuffers(1, &g_s.quadEBO);
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create line drawing shader:
    {
        const char* vertexShader = R"(
            #version 430 core

            layout (location = 0) in vec3 a_pos;

            uniform mat4 u_mvp;

            void main() {
                gl_Position = u_mvp * vec4(a_pos.xyz, 1.0);
            }
        )";
        const char* fragmentShader = R"(
            #version 430 core

            out vec4 fragColor;

            uniform vec4 color;

            void main() {
                fragColor = color;
            }
        )";
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexShader, fragmentShader));
    }

    // Create quad vertices:
    {
        f32 verts[] = {
            -1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
        };

        i32 indices[] = {
            0, 1, 2,
            2, 3, 0,
        };

        g_s.quadIndicesCount = sizeof(indices) / sizeof(indices[0]);

        glGenBuffers(1, &g_s.quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glGenBuffers(1, &g_s.quadEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &g_s.quadVAO);
        glBindVertexArray(g_s.quadVAO);
        constexpr i32 stride = 3 * sizeof(f32);
        constexpr i32 dimmensions = 3;
        u32 attribPosLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_pos"));
        glVertexAttribPointer(attribPosLoc, dimmensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(attribPosLoc);
    }

    g_s.viewMat = core::mat4f::identity();
    g_s.projectionMat = core::ortho(-1, 1, -1, 1, -1, 1);

    // Create cells:
    {
        for (u32 i = 0; i < g_s.cellCount; ++i) {
            Cell& cell = g_s.cells[i];
            cell.isOn = false;
            cell.pos = core::v(f32(i % g_s.cellsPerRow) * (g_s.cellWidth) + g_s.gridOffset.x(),
                               f32(i / g_s.cellsPerRow) * (g_s.cellHeight) + g_s.gridOffset.y());
        }

        g_s.cells[0].isOn = true;
        g_s.cells[1].isOn = true;
        g_s.cells[2].isOn = true;
        g_s.cells[3].isOn = true;
        g_s.cells[4].isOn = true;
    }

    // Create line vertices:
    {
        f32 verts[] = {
            -1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f,
        };

        glGenBuffers(1, &g_s.lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glGenVertexArrays(1, &g_s.lineVAO);
        glBindVertexArray(g_s.lineVAO);
        constexpr i32 stride = 3 * sizeof(f32);
        constexpr i32 dimmensions = 3;
        u32 attribPosLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_pos"));
        glVertexAttribPointer(attribPosLoc, dimmensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(attribPosLoc);
    }

    return {};
}

namespace {

void renderCell(const Cell& cell, const core::vec4f& color) {
    State& g_s = state();

    auto model = core::mat4f::identity();
    auto pos = g_s.worldSpaceGrid.convertTo_v(cell.pos, g_s.viewSpaceGrid);
    f32 width = core::blend(g_s.worldSpaceGrid.min.x(), g_s.worldSpaceGrid.max.x(), 0.0f, 1.0f, f32(g_s.cellWidth));
    f32 height = core::blend(g_s.worldSpaceGrid.min.y(), g_s.worldSpaceGrid.max.y(), 0.0f, 1.0f, f32(g_s.cellHeight));
    model = core::scale(model, core::v(width, height, 1.0f));
    model = core::translate(model, core::v(pos.x(), pos.y(), 0.0f));

    auto mvp = g_s.projectionMat * g_s.viewMat * model;
    g_s.shaderProg.setUniform_m("u_mvp", mvp);
    g_s.shaderProg.setUniform_v("color", color);

    if (cell.isOn) glDrawElements(GL_TRIANGLES, g_s.quadIndicesCount, GL_UNSIGNED_INT, 0);
    else           glDrawElements(GL_LINE_LOOP, g_s.quadIndicesCount, GL_UNSIGNED_INT, 0);
}

void renderLine(f32 x1, f32 y1, f32 x2, f32 y2, f32 lineWidth, const core::vec4f& color) {
    State& g_s = state();
    auto start = g_s.worldSpaceGrid.convertTo_v(core::v(x1, y1), g_s.viewSpaceGrid);
    auto end = g_s.worldSpaceGrid.convertTo_v(core::v(x2, y2), g_s.viewSpaceGrid);
    core::vec2f diff = end - start;
    core::vec2f mid = (start + end) / 2;
    f32 lineLen = static_cast<f32>(diff.length());
    f32 halfLen = lineLen / 2.0f;
    f32 lineAngle = core::slope_to_deg(x1, y1, x2, y2);

    g_s.shaderProg.use();
    glBindVertexArray(g_s.quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBO);

    auto model = core::mat4f::identity();
    model = core::scale(model, core::v(halfLen, lineWidth, 1.0f)); // scale the quad to be a line
    model = core::rotateZ_right(model, core::deg_to_rad(lineAngle));
    model = core::translate(model, core::v(mid.x(), mid.y(), 0.0f));

    auto mvp = g_s.projectionMat * g_s.viewMat * model;
    g_s.shaderProg.setUniform_m("u_mvp", mvp);
    g_s.shaderProg.setUniform_v("color", color);

    glDrawElements(GL_TRIANGLES, g_s.quadIndicesCount, GL_UNSIGNED_INT, 0);
}

} // namespace

void mainLoop(CommonState& commonState) {
    State& g_s = state();

    resetOpenGLBinds();
    {
        g_s.shaderProg.use();
        glBindVertexArray(g_s.quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBO);

        for (u32 i = 0; i < g_s.cellCount; ++i) {
            Cell& cell = g_s.cells[i];
            renderCell(cell, core::BLACK);
        }
    }

    resetOpenGLBinds();
    {
        constexpr f32 lineWidth = 0.01f;
        g_s.ray = { core::v(0.0f, 0.0f), core::v(1.0f, 1.0f) };
        auto end = g_s.ray.at(1000.0f);
        renderLine(g_s.ray.origin.x(), g_s.ray.origin.y(), end.x(), end.y(), lineWidth, core::RED);
    }

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);
    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
}

} // namespace ray_in_voxel_space

