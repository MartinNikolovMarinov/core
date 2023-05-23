#include "ray_in_voxel_space.h"

#include <keyboard.h>
#include <mouse.h>
#include <keyboard.h>
#include <shader_prog.h>
#include <grid.h>
#include <bbox.h>

#include <std/stringer.h>

namespace ray_in_voxel_space {

namespace {

struct Cell {
    core::vec2f pos = {};
    bool isOn = false;
};

struct State {
    static constexpr Grid2D viewSpaceGrid  = { core::v(-1.0f, 1.0f), core::v(1.0f, -1.0f) };
    static constexpr Grid2D worldSpaceGrid = { core::v(0.0f, 0.0f), core::v(1000.0f, 1000.0f) };
    // static constexpr core::mat4f worldSpaceToViewSpaceMatrix = worldSpaceGrid.getToConvMatrix(viewSpaceGrid);
    static constexpr bool renderGrid = false;

    Mouse mouse;
    Keyboard keyboard;
    u64 lastMouseClickTimeMs = 0;

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
    static constexpr u32 cellsPerCol = cellCount / cellsPerRow;
    Cell cells[cellCount] = {};
    Bbox2D cellsBbox;

    u32 lineVAO = 0;
    u32 lineVBO = 0;

    core::vec2f a = core::uniform<2, f32>(-1.f);
    core::vec2f b = core::uniform<2, f32>(-1.f);
    bool canRenderLine = false;

    Cell& getCellAt(u32 x, u32 y) {
        Assert(x < cellsPerRow, "Invalid x!");
        Assert(y < cellsPerCol, "Invalid y!");
        return cells[y * cellsPerRow + x];
    }
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

void handleMousePress() {
    State& g_s = state();
    Mouse& mouse = g_s.mouse;
    constexpr i32 delayMs = 5;
    auto currTimeMs = ValueOrDie(core::os_unix_time_stamp_in_ms());

    // Left mouse button controls point a:
    if (mouse.leftButton.isPressed() && currTimeMs > g_s.lastMouseClickTimeMs) {
        g_s.lastMouseClickTimeMs = currTimeMs + delayMs;
        g_s.a = core::v(f32(mouse.x), f32(mouse.y));
        if (g_s.b.x() != -1) g_s.canRenderLine = true;
    }

    // Right mouse button cotnrols point b:
    if (mouse.rightButton.isPressed() && currTimeMs > g_s.lastMouseClickTimeMs) {
        g_s.lastMouseClickTimeMs = currTimeMs + delayMs;
        g_s.b = core::v(f32(mouse.x), f32(mouse.y));
        if (g_s.a.x() != -1) g_s.canRenderLine = true;
    }
}

void handleUserInput() {
    handleMousePress();
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
        State& g_s = state();
        KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
        g_s.keyboard.setModifiers(core::move(keyModifiers)).setKey(core::move(keyInfo));

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

    // Mouse event handlers:

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow*, f64 xpos, f64 ypos) {
        State& g_s = state();
        xpos = core::blend(0.0f, f32(g_s.viewportWidth), g_s.worldSpaceGrid.min.x(), g_s.worldSpaceGrid.max.x(), f32(xpos));
        ypos = core::blend(0.0f, f32(g_s.viewportHeight), g_s.worldSpaceGrid.min.y(), g_s.worldSpaceGrid.max.y(), f32(ypos));
        xpos = core::round_to(xpos, 3);
        ypos = core::round_to(ypos, 3);
        g_s.mouse.setPos(xpos, ypos);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetCursorPosCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow*, i32 button, i32 action, i32) {
        State& g_s = state();
        KeyInfo mouseButton = KeyInfo::createFromGLFW(button, 0, action);
        g_s.mouse.setButton(mouseButton);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetMouseButtonCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    // Resize event handler:
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
        core::vec2f gridShift = g_s.worldSpaceGrid.center() - core::uniform<2, f32>(g_s.cellsPerRow * g_s.cellWidth / 2);
        for (u32 i = 0; i < g_s.cellCount; ++i) {
            Cell& cell = g_s.cells[i];
            cell.isOn = false;
            cell.pos = core::v(f32(i % g_s.cellsPerRow) * (g_s.cellWidth) + gridShift.x(),
                               f32(i / g_s.cellsPerRow) * (g_s.cellHeight) + gridShift.y());
        }

        g_s.cells[0 + 11 * g_s.cellsPerRow].isOn = true;
        g_s.cells[1 + 11 * g_s.cellsPerRow].isOn = true;
        g_s.cells[2 + 11 * g_s.cellsPerRow].isOn = true;
        g_s.cells[3 + 11 * g_s.cellsPerRow].isOn = true;
        g_s.cells[4 + 11 * g_s.cellsPerRow].isOn = true;

        g_s.cellsBbox = Bbox2D(g_s.cells[0].pos, g_s.cells[g_s.cellCount - 1].pos + core::v(f32(g_s.cellWidth), f32(g_s.cellHeight)));
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

void bindVertexArray_memorized(u32 vao) {
    static u32 lastBoundVAO = 0;
    if (lastBoundVAO != vao) {
        glBindVertexArray(vao);
        lastBoundVAO = vao;
    }
}

void bindArrayBuffer_memorized(u32 vbo) {
    static u32 lastBoundVBO = 0;
    if (lastBoundVBO != vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        lastBoundVBO = vbo;
    }
}

void bindElementArrayBuffer_memorized(u32 ebo) {
    static u32 lastBoundEBO = 0;
    if (lastBoundEBO != ebo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        lastBoundEBO = ebo;
    }
}

core::mat4f renderModel_quad2D(f32 x, f32 y, f32 z, f32 width, f32 height) {
    auto model = core::mat4f::identity();
    model = core::scale(model, core::v(width, height, 1.0f));
    model = core::translate(model, core::v(x, y, z));
    return model;
}

core::mat4f renderModel_line(f32 x1, f32 y1, f32 x2, f32 y2, f32 lineWidth) {
    auto start = core::v(x1, y1);
    auto end = core::v(x2, y2);
    core::vec2f diff = end - start;
    core::vec2f mid = (start + end) / 2;
    f32 lineLen = static_cast<f32>(diff.length());
    f32 halfLen = lineLen / 2.0f;
    f32 lineAngle = core::slope(x1, -y1, x2, -y2);
    auto model = core::mat4f::identity();
    model = core::scale(model, core::v(halfLen, lineWidth, 1.0f)); // scale the quad to be a line
    model = core::rotateZ_right(model, lineAngle);
    model = core::translate(model, core::v(mid.x(), mid.y(), 0.0f));
    return model;
}

void renderQuad(f32 x, f32 y, f32 width, f32 height, const core::vec4f& color, bool fill) {
    State& g_s = state();

    auto topLeft = core::v(x + width / 2, y + height / 2);
    auto pos = g_s.worldSpaceGrid.convertTo_v(topLeft, g_s.viewSpaceGrid);
    width = core::blend(g_s.worldSpaceGrid.min.x(), g_s.worldSpaceGrid.max.x(), 0.0f, 1.0f, width);
    height = core::blend(g_s.worldSpaceGrid.min.y(), g_s.worldSpaceGrid.max.y(), 0.0f, 1.0f, height);
    auto model = renderModel_quad2D(pos.x(), pos.y(), 0.0f, width, height);

    auto mvp = g_s.projectionMat * g_s.viewMat * model;
    g_s.shaderProg.setUniform_m("u_mvp", mvp);
    g_s.shaderProg.setUniform_v("color", color);

    g_s.shaderProg.use();
    bindVertexArray_memorized(g_s.quadVAO);
    bindArrayBuffer_memorized(g_s.quadVBO);
    bindElementArrayBuffer_memorized(g_s.quadEBO);

    if (fill) glDrawElements(GL_TRIANGLES, g_s.quadIndicesCount, GL_UNSIGNED_INT, 0);
    else      glDrawElements(GL_LINE_LOOP, g_s.quadIndicesCount, GL_UNSIGNED_INT, 0);
}

void renderCell(const Cell& cell, const core::vec4f& color) {
    State& g_s = state();
    renderQuad(cell.pos.x(), cell.pos.y(), g_s.cellWidth, g_s.cellHeight, color, cell.isOn);
}

void renderLine(f32 x1, f32 y1, f32 x2, f32 y2, f32 lineWidth, const core::vec4f& color) {
    State& g_s = state();

    g_s.shaderProg.use();
    bindVertexArray_memorized(g_s.quadVAO);
    bindArrayBuffer_memorized(g_s.quadVBO);
    bindElementArrayBuffer_memorized(g_s.quadEBO);

    auto start = g_s.worldSpaceGrid.convertTo_v(core::v(x1, y1), g_s.viewSpaceGrid);
    auto end = g_s.worldSpaceGrid.convertTo_v(core::v(x2, y2), g_s.viewSpaceGrid);
    lineWidth = core::blend(g_s.worldSpaceGrid.min.x(), g_s.worldSpaceGrid.max.x(), 0.0f, 1.0f, lineWidth);
    auto model = renderModel_line(start.x(), start.y(), end.x(), end.y(), lineWidth);

    auto mvp = g_s.projectionMat * g_s.viewMat * model;
    g_s.shaderProg.setUniform_m("u_mvp", mvp);
    g_s.shaderProg.setUniform_v("color", color);

    glDrawElements(GL_TRIANGLES, g_s.quadIndicesCount, GL_UNSIGNED_INT, 0);
}

void renderGrid(const Grid2D& grid, u32 rows, u32 cols, f32 lineWidth, const core::vec4f& color) {
    State& g_s = state();
    f32 rowDist = grid.max.x() / f32(rows);
    for (u32 i = 0; i <= rows; i++) {
        renderLine(rowDist * f32(i), g_s.worldSpaceGrid.min.x(), rowDist * f32(i), g_s.worldSpaceGrid.max.x(), lineWidth, color);
    }
    f32 colDist = grid.max.y() / f32(cols);
    for (u32 i = 0; i <= cols; i++) {
        renderLine(g_s.worldSpaceGrid.min.y(), colDist * f32(i), g_s.worldSpaceGrid.max.y(), colDist * f32(i), lineWidth, color);
    }
}

core::tuple<i32, i32> toCellCoordinates(const core::vec2d& x) {
    State& g_s = state();
    auto cellsDelta = core::v(f64(g_s.cellWidth), f64(g_s.cellHeight));
    auto t = (x -  core::v_conv<core::vec2d>(g_s.cellsBbox.min)) / cellsDelta;
    i32 cellX = i32(core::clamp(t.x(), 0.0, f64(g_s.cellsPerRow - 1)));
    i32 cellY = i32(core::clamp(t.y(), 0.0, f64(g_s.cellsPerCol - 1)));
    return core::create_tuple(std::move(cellX), std::move(cellY));
}

} // namespace

void mainLoop(CommonState& commonState) {
    State& g_s = state();

    g_s.mouse.clear();
    // FIXME: Modifiers work totally wrong. Need to fix them! Just fix the keyboard handler nightmare!
    g_s.keyboard.clear();
    handleUserInput();

    if (g_s.canRenderLine) {
        constexpr f32 lineWidth = 3;
        auto res = g_s.cellsBbox.intersectionWithLineSegment(g_s.a, g_s.b);
        if (res.hasEntry && res.hasExit) {
            auto start = g_s.a;
            auto end = res.entry;
            renderLine(start.x(), start.y(), end.x(), end.y(), lineWidth, core::GREEN);

            start = res.entry;
            end = res.exit;
            renderLine(start.x(), start.y(), end.x(), end.y(), lineWidth, core::RED);

            start = res.exit;
            end = g_s.b;
            renderLine(start.x(), start.y(), end.x(), end.y(), lineWidth, core::GREEN);
        }
        else if (res.hasEntry) {
            auto start = g_s.a;
            auto end = res.entry;
            renderLine(start.x(), start.y(), end.x(), end.y(), lineWidth, core::GREEN);

            start = res.entry;
            end = g_s.b;
            renderLine(start.x(), start.y(), end.x(), end.y(), lineWidth, core::RED);
        }
        else if (res.hasExit) {
            auto start = g_s.a;
            auto end = res.exit;
            renderLine(start.x(), start.y(), end.x(), end.y(), lineWidth, core::RED);

            start = res.exit;
            end = g_s.b;
            renderLine(start.x(), start.y(), end.x(), end.y(), lineWidth, core::GREEN);
        }
        else {
            renderLine(g_s.a.x(), g_s.a.y(), g_s.b.x(), g_s.b.y(), lineWidth, core::GREEN);
        }

        if(res.hasEntry) {
            core::vec2d src = core::v_conv<core::vec2d>(res.entry);
            core::vec2d end = res.hasExit ? core::v_conv<core::vec2d>(res.exit) : core::v_conv<core::vec2d>(g_s.b);
            core::vec2d dir = end - src;
            core::vec2d invDir = core::v(dir.x() != 0 ? 1 / dir.x() : core::MAX_F32,
                                  dir.y() != 0 ? 1 / dir.y() : core::MAX_F32);
            core::vec2d size = core::v(f64(g_s.cellWidth), f64(g_s.cellHeight));
            core::vec2i step = core::v(dir.x() > 0 ? 1 : -1,
                                       dir.y() > 0 ? 1 : -1);

            core::vec2d idx = src / size;
            core::vec2d entryIdxUp = core::vceil(idx);
            core::vec2d entryIdxDown = core::vfloor(idx);
            core::vec2d tUp = (size * entryIdxUp - src) * invDir;
            core::vec2d tDown = (size * entryIdxDown - src) * invDir;

            core::vec2d tU = size * core::vabs(invDir);
            core::vec2d t = core::vmax(tUp, tDown);

            core::vec2i cellIdx;
            auto ret = toCellCoordinates(size * entryIdxDown);
            cellIdx.x() = core::move(ret.get<0>());
            cellIdx.y() = core::move(ret.get<1>());

            f64 curT = core::min(t.x(), t.y());
            f64 maxT = core::min(end.x(), end.y());

            auto loopCondition = [&]() {
                // Spliting the condition into two parts to simplify debugging.
                bool ret = true;
                ret &= 0 <= cellIdx.x() && cellIdx.x() < i32(g_s.cellsPerRow);
                ret &= 0 <= cellIdx.y() && cellIdx.y() < i32(g_s.cellsPerCol);
                ret &= curT < maxT;
                return ret;
            };

            while (loopCondition()) {
                auto &cell = g_s.cells[i32(cellIdx.x()) + i32(cellIdx.y()) * g_s.cellsPerRow];
                if (cell.isOn) {
                    renderCell(cell, core::BLUE);
                    break;
                }
                cell.isOn = true;
                renderCell(cell, core::BLUE);
                cell.isOn = false;

                if (t.x() < t.y()) {
                    curT = t.x();
                    t.x() += tU.x();
                    cellIdx.x() += step.x();
                }
                else {
                    curT = t.y();
                    t.y() += tU.y();
                    cellIdx.y() += step.y();
                }
            }
        }

        // TODO: If the ray is inside the grid there I should just find the starting cell and start from there.
    }

    // Render Cells:
    {
        for (u32 i = 0; i < g_s.cellCount; ++i) {
            Cell& cell = g_s.cells[i];
            renderCell(cell, core::BLACK);
        }

        // Render bounding box:
        renderQuad(g_s.cellsBbox.min.x(), g_s.cellsBbox.min.y(), g_s.cellsBbox.width(), g_s.cellsBbox.height(), core::BLACK, false);
    }

    // Render Grid:
    if (g_s.renderGrid) {
        renderGrid(g_s.worldSpaceGrid, 10, 10, 1, core::RED);
    }

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);

    // DEBUG LOGGING:
    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
    // fmt::print("Mouse: pos:{},{}; right_btn: is_pressed:{};\n", g_s.mouse.x, g_s.mouse.y, g_s.mouse.leftButton.isPressed());
}

} // namespace ray_in_voxel_space

