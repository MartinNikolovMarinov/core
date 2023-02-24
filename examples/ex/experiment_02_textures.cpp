#include "experiment_02_textures.h"

#include <keyboard.h>
#include <shader_prog.h>
#include <mouse.h>

namespace textures_ex_02 {

namespace {

struct State {
    ShaderProg shaderProg;
};

State& state() {
    static State g_state = {};
    return g_state;
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

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

void destroy() {}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create shader program:
    {
        const char* vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 in_pos;

            void main() {
                gl_Position = vec4(in_pos.xyz, 1.0);
            }
        )";
        const char* fragShaderSource = R"(
            #version 330 core

            out vec4 out_fragColor;
            uniform vec4 u_color;

            void main() {
                out_fragColor = u_color;
            }
        )";
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexShaderSource, fragShaderSource));
    }

    return {};
}

void mainLoop(CommonState& commonState) {
    // Debug reset binds to make sure everyting is bound correctly later on.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    g_s.shaderProg.use();

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);
    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
}

} // namespace textures_ex_02
