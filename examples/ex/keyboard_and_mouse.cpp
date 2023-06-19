#include "keyboard_and_mouse.h"

#include <keyboard.h>
#include <mouse.h>
#include <glfw/glfw_impl.h>

namespace keyboard_and_mouse {

using Mouse = app::Mouse;
using Keyboard = app::GLFWKeyboard;

namespace {

struct State {
    Keyboard keyboard;
    Mouse mouse;
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        State& g_s = state();
        app::KeyboardModifiers keyModifiers = app::createKeyboardModifiersGLFW(mods);
        app::KeyInfo keyInfo = app::createKeyInfoGLFW(key, scancode, action);
        if (keyInfo.isReleased() && keyInfo.isModifier()) {
            g_s.keyboard.modifiers.remove(keyModifiers);
        }
        else {
            g_s.keyboard.modifiers.combine(keyModifiers);
        }
        g_s.keyboard.setKey(core::move(keyInfo));


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

    glfwSetCharCallback(glfwWindow, [](GLFWwindow*, u32 codepoint) {
        State& g_s = state();
        g_s.keyboard.setTextInput(codepoint);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW CharCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow*, i32 button, i32 action, i32) {
        State& g_s = state();
        app::KeyInfo mouseButton = app::createKeyInfoGLFW(button, 0, action);
        app::setGLFWMouseButton(g_s.mouse, mouseButton);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set glfwSetMouseButtonCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow*, f64 xpos, f64 ypos) {
        State& g_s = state();
        g_s.mouse.setPos(xpos, ypos);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW CursorPosCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetCursorEnterCallback(glfwWindow, [](GLFWwindow*, i32 entered) {
        State& g_s = state();
        g_s.mouse.isInWindow = (entered == GL_TRUE);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW CursorEnterCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetScrollCallback(glfwWindow, [](GLFWwindow*, f64 xoffset, f64 yoffset) {
        State& g_s = state();
        g_s.mouse.setScroll(xoffset, yoffset);
    });

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

void destroy() {
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    return {};
}

void mainLoop(CommonState&) {
    State& g_s = state();

    fmt::print("Keyboard: {}\n", g_s.keyboard.toString());
    fmt::print("Mouse: {}\n", g_s.mouse.toString());

    g_s.keyboard.update();
    g_s.mouse.update();
}

} // namespace keyboard_and_mouse
