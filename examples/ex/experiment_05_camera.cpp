#include "experiment_05_camera.h"

#include <keyboard.h>
#include <mouse.h>
#include <shader_prog.h>
#include <box.h>
#include <glfw/glfw_impl.h>

#include <std/stringer.h>

namespace camera_ex_05 {

namespace {

using Mouse = app::Mouse;
using Keyboard = app::GLFWKeyboard;

struct Cell {
    core::vec2f pos = {};
    bool isOn = false;
};

struct State {
    static constexpr bool renderGrid = false;

    Mouse mouse;
    Keyboard keyboard;

    i32 viewportWidth;
    i32 viewportHeight;

    ShaderProg shaderProg;
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    State& g_s = state();
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

    g_s.viewportWidth = s.mainWindow.width;
    g_s.viewportHeight = s.mainWindow.height;

    glEnable(GL_DEPTH_TEST);

    // Key event handler:
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        [[maybe_unused]] app::KeyboardModifiers keyModifiers = app::createKeyboardModifiersGLFW(mods);
        app::KeyInfo keyInfo = app::createKeyInfoGLFW(key, scancode, action);
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

    // Resize event handler:
    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow*, i32 width, i32 height) {
        State& g_s = state();
        g_s.viewportWidth = width;
        g_s.viewportHeight = height;
        glViewport(0, 0, width, height);
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
    auto& g_s = state();
    immediate_cubeRendererDestroy();
    g_s.shaderProg.destroy();
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create the cube shader:
    {
        const char* vertexShader = R"(
            #version 430 core

            layout (location = 0) in vec3 in_pos;

            uniform mat4 u_projection;
            uniform mat4 u_view;
            uniform mat4 u_model;

            void main() {
                gl_Position = u_projection * u_view * u_model * vec4(in_pos.xyz, 1.0);
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

    u32 inPosAttribLocation = ValueOrDie(g_s.shaderProg.getAttribLocation("in_pos"));
    immediate_cubeRendererInit(inPosAttribLocation);

    return {};
}

namespace {

void clearGLState() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

} // namespace

void mainLoop(CommonState& commonState) {
    State& g_s = state();

    clearGLState();

    auto proj = core::mat4x4f::identity();
    proj = core::perspective(core::deg_to_rad(45.0f), f32(g_s.viewportWidth) / f32(g_s.viewportHeight), 0.1f, 100.0f);

    auto view = core::mat4x4f::identity();
    view = core::translate(view, core::v(0.0f, 0.0f, -3.0f));

    g_s.shaderProg.use();
    g_s.shaderProg.setUniform_view("u_view", view);
    g_s.shaderProg.setUniform_proj("u_projection", proj);

    // Box Scene :

    {
        Box b;
        b.scale(core::uniform<3, f32>(0.3f));
        b.rotate(core::Z_AXIS, core::radians(core::deg_to_rad(45.0f)));
        b.translate(core::v(0.0f, -0.2f, 0.0f));
        b.setColor(core::v(1.0f, 0.0f, 0.0f, 1.0f));
        immediate_render(g_s.shaderProg, b);
    }

    {
        Box b;
        b.scale(core::v(0.5f, 0.3f, 0.3f));
        b.rotate(core::Y_AXIS, core::radians(core::deg_to_rad(45.0f)));
        b.rotate(core::X_AXIS, core::radians(core::deg_to_rad(20.0f)));
        b.rotate(core::Z_AXIS, core::radians(core::deg_to_rad(30.0f)));
        b.translate(core::v(0.7f, 0.2f, 0.0f));
        b.setColor(core::v(0.0f, 0.0f, 1.0f, 1.0f));
        immediate_render(g_s.shaderProg, b);
    }

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);
    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
}

} // namespace camera_ex_05

