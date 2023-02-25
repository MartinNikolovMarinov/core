#include "experiment_app.h"

#include <grid.h>
#include <keyboard.h>
#include <mouse.h>
#include <shader_prog.h>
#include <shape.h>
namespace app {

namespace {

struct AppState {
    // Constants:
    static constexpr Grid2D clipSpaceGrid    = { core::v(-1.0f, -1.0f), core::v(1.0f, 1.0f) };
    static constexpr Grid2D worldSpaceGrid   = { core::v(0.0f, 0.0f), core::v(1000.0f, 1000.0f) };
    static constexpr core::vec2f worldCenter = core::v(worldSpaceGrid.max.x() / 2, worldSpaceGrid.max.y() / 2);

    Keyboard keyboardState;
    Mouse mouseState;
    bool keyboardStateChange;
    bool mouseStateChange;
    bool windowStateChange;
    u64 lastWindowDragTimestamp_ms;
    ShaderProg guiShader;
    core::arr<Shape2D> shapesToRender;
};

AppState& state(bool clear = false) {
    static AppState g_state = {};
    if (clear) g_state = {};
    return g_state;
}

core::expected<GraphicsLibError> initEventHandlers(GLFWwindow* window) {
    const char* errDesc = nullptr;

    // Error handler

    glfwSetErrorCallback([](i32, const char* description) {
        fmt::print(stderr, "GLFW error: {}\n", description);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetErrorCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    // Keyboard event handlers

    glfwSetKeyCallback(window, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        auto& g_s = app::state();
        KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
        g_s.keyboardState.setModifiers(core::move(keyModifiers))
                                  .setKey(core::move(keyInfo));

        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.isPressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        g_s.keyboardStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetKeyCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetCharCallback(window, [](GLFWwindow*, u32 codepoint) {
        auto& g_s = app::state();
        g_s.keyboardState.setTextInput(core::rune(codepoint));
        g_s.keyboardStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetCharCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    // Mouse event handlers

    glfwSetCursorPosCallback(window, [](GLFWwindow*, f64 xpos, f64 ypos) {
        auto& g_s = app::state();
        g_s.mouseState.setPos(xpos, ypos);
        g_s.mouseStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetCursorPosCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetCursorEnterCallback(window, [](GLFWwindow*, i32 entered) {
        auto& g_s = app::state();
        g_s.mouseState.setInWindow(entered != 0);
        g_s.mouseStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetCursorEnterCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetMouseButtonCallback(window, [](GLFWwindow*, i32 button, i32 action, i32 mods) {
        auto& g_s = app::state();
        // Change keyboard state to take account of the modifiers:
        KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        g_s.keyboardState.setModifiers(core::move(keyModifiers));
        g_s.keyboardStateChange = true;
        // Then change the mouse state:
        KeyInfo mouseButton = KeyInfo::createFromGLFW(button, 0, action);
        g_s.mouseState.setButton(mouseButton);
        g_s.mouseStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetMouseButtonCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetScrollCallback(window, [](GLFWwindow*, f64 xoffset, f64 yoffset) {
        auto& g_s = app::state();
        g_s.mouseState.setScroll(xoffset, yoffset);
        g_s.mouseStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetScrollCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetDropCallback(window, [](GLFWwindow*, i32 count, const char** paths) {
        // TODO: save this to the global state.
        fmt::print("Dropped {} files.\n", count);
        fmt::print("Paths:\n");
        for (i32 i = 0; i < count; ++i) {
            fmt::print("  {}\n", paths[i]);
        }
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetDropCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    // Window event handlers

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, i32 width, i32 height) {
        auto& g_s = app::state();
        auto& commonState = common::state();
        fmt::print("Window Resize to: {}, {}\n", width, height);
        commonState.mainWindow.width = width;
        commonState.mainWindow.height = height;
        glViewport(0, 0, width, height);
        g_s.windowStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetFramebufferSizeCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetWindowPosCallback(window, [](GLFWwindow*, i32 xpos, i32 ypos) {
        auto& g_s = app::state();
        u64 now = ValueOrDie(core::os_unix_time_stamp_in_ms());
        u64 diff = now - g_s.lastWindowDragTimestamp_ms;
        static constexpr u32 throttleDragEventsTime_ms = 50;
        if (diff > throttleDragEventsTime_ms) {
            fmt::print("Window dragged to: {}, {}\n", xpos, ypos);
            g_s.lastWindowDragTimestamp_ms = ValueOrDie(core::os_unix_time_stamp_in_ms());
            g_s.windowStateChange = true;
        }
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetWindowPosCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        auto& g_s = app::state();
        fmt::print("Window close requested\n");
        glfwSetWindowShouldClose(window, true);
        g_s.windowStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetWindowCloseCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetWindowFocusCallback(window, [](GLFWwindow*, i32 focused) {
        auto& g_s = app::state();
        fmt::print("Window focus changed to: {}\n", focused ? "true" : "false");
        g_s.windowStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetWindowFocusCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetWindowMaximizeCallback(window, [](GLFWwindow*, i32 maximized) {
        auto& g_s = app::state();
        fmt::print("Window maximized changed to: {}\n", maximized ? "true" : "false");
        g_s.windowStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetWindowMaximizeCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetWindowContentScaleCallback(window, [](GLFWwindow*, f32 xscale, f32 yscale) {
        auto& g_s = app::state();
        fmt::print("Window content scale: {}, {}\n", xscale, yscale);
        g_s.windowStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetWindowContentScaleCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetWindowRefreshCallback(window, [](GLFWwindow*) {
        auto& g_s = app::state();
        fmt::print("Window refresh requested\n");
        g_s.windowStateChange = true;
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetWindowRefreshCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    return {};
}

ShaderProg createGUIShaderProgram() {
    const char* vertexGUIShaderSource = R"(
        #version 330 core

        layout (location = 0) in vec3 in_pos;

        void main() {
            gl_Position = vec4(in_pos.xyz, 1.0);
        }
    )";
    const char* fragmentGUIShaderSource = R"(
        #version 330 core

        out vec4 out_fragColor;
        uniform vec4 u_color;

        void main() {
            out_fragColor = u_color;
        }
    )";
    ShaderProg guiShader = ValueOrDie(ShaderProg::create(vertexGUIShaderSource, fragmentGUIShaderSource));
    return guiShader;
}

bool shouldRender() {
    auto& g_s = app::state();
    bool shouldRender = g_s.windowStateChange || g_s.mouseStateChange || g_s.keyboardStateChange;
    return shouldRender;
}

void prepareInputStateForNextFrame() {
    auto& g_s = app::state();
    g_s.keyboardStateChange = false;
    g_s.mouseStateChange = false;
    g_s.windowStateChange = false;
    g_s.keyboardState.clear();
    g_s.mouseState.clear();
}

void renderShape(Shape2D& shape) {
    auto& g_s = app::state();

    // Bind shape:
    glBindBuffer(GL_ARRAY_BUFFER, shape.vboID());
    glBindVertexArray(shape.vaoID());

    // Set uniforms:
    Check(g_s.guiShader.setUniform_v("u_color", shape.color()));

    switch (shape.renderMode().mode) {
        case Shape2D::RenderMode::Mode::TRIANGLE_FAN:
            glDrawArrays(GL_TRIANGLE_FAN, 0, shape.vertexCount());
            break;
        case Shape2D::RenderMode::Mode::TRIANGLES:
            glDrawArrays(GL_TRIANGLES, 0, shape.vertexCount());
            break;
        default:
            Assert(false, fmt::format("Unknown render mode: {}", shape.renderMode().mode).c_str());
            return;
    }
}

[[maybe_unused]] void debug__printVertexArr(const core::arr<core::vec2f>& vertices) {
    auto& g_s = app::state();
    for(ptr_size i = 0; i < vertices.len(); ++i) {
        const auto& p = vertices[i];
        auto vv = convertVecUsingGrid(p, g_s.worldSpaceGrid, g_s.clipSpaceGrid);
        fmt::print("v_{}: (x:{}, y:{})\n", i, vv.x(), vv.y());
    }
}

}

core::expected<GraphicsLibError> init(CommonState& commonState) {
    Assert(commonState.mainWindow.glfwWindow);
    ValueOrReturn(initEventHandlers(commonState.mainWindow.glfwWindow));
    return {};
}

void destroy() {
    auto& g_s = app::state();
    g_s.guiShader.destroy();
    for (ptr_size i = 0; i < g_s.shapesToRender.len(); ++i) {
        g_s.shapesToRender[i].destroy();
    }
    // clear the state:
    app::state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    auto& g_s = app::state();
    g_s.guiShader = createGUIShaderProgram();

    Shape2D::VertexLayout vl;
    vl.stride = sizeof(core::vec2f);
    vl.offset = 0;
    vl.usage = { Shape2D::Usage::Access::STATIC, Shape2D::Usage::AccessType::DRAW };
    vl.posAttribId = ValueOrDie(g_s.guiShader.getAttribLocation("in_pos"));
    vl.renderMode.mode = Shape2D::RenderMode::TRIANGLES;

    Shape2D rectShape = Shape2D::createRect2D(vl, core::v(-0.1f, 0.1f), 1.0f, 1.0f,
                                              core::v(255u, 0u, 0u, 255u), 1.0f);

    Shape2D triangleShape = Shape2D::createTriangle2D(vl,
                                                      core::v(-0.5f, -0.5f),
                                                      core::v(0.5f, -0.5f),
                                                      core::v(0.0f, 0.5f),
                                                      core::v(0u, 255u, 0u, 255u),
                                                      1.0f);

    vl.renderMode.mode = Shape2D::RenderMode::TRIANGLE_FAN;

    Shape2D circleShape = Shape2D::createCircle2D(vl, core::v(0.f, 0.f), 0.4f, 100,
                                                  core::v(255u, 255u, 255u, 255u), 0.0f);

    core::arr<Shape2D> shapesToRender(0, 3);
    shapesToRender.append(core::move(rectShape));
    shapesToRender.append(core::move(triangleShape));
    shapesToRender.append(core::move(circleShape));
    g_s.shapesToRender = core::move(shapesToRender);

    return {};
}

void mainLoop(CommonState& commonState) {
    auto& g_s = app::state();

    if (shouldRender()) {
        // Debug reset binds to make sure everyting is bound correctly later on.
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);

        g_s.guiShader.use();

        for (ptr_size i = 0; i < g_s.shapesToRender.len(); ++i) {
            renderShape(g_s.shapesToRender[i]);
        }

        glfwSwapBuffers(commonState.mainWindow.glfwWindow);

        // DEBUG PRINT STATE:
        if (g_s.keyboardStateChange) {
            fmt::print("{}", g_s.keyboardState.toString());
        }
        if (g_s.mouseStateChange) {
            fmt::print("{}", g_s.mouseState.toString());
        }

        prepareInputStateForNextFrame();
    }
}

} // namespace app
