#include "keyboard.h"
#include "mouse.h"

#include <core.h>
#include <std/core.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>

using namespace coretypes;

// GLOBAL STATE:
static keyboard g_keyboard;
static mouse g_mouse;
static bool g_render = true;
static bool g_keyboardStateChange = true;
static bool g_mouseStateChange = true;
static bool g_windowStateChange = true;
static std::chrono::time_point<std::chrono::high_resolution_clock> g_lastWindowDragTime;

i32 main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Example Window", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    glfwSetErrorCallback([](i32, const char* description) {
        std::cerr << "GLFW Error: " << description << std::endl;
    });

    // Keyboard callbacks

    glfwSetKeyCallback(window, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        keyboard_modifiers keyModifiers = keyboard_modifiers::create_from_glfw(mods);
        key_info keyInfo = key_info::create_from_glfw(key, scancode, action);
        g_keyboard.set_modifiers(core::move(keyModifiers))
                  .set_key(core::move(keyInfo));

        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.is_pressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        g_keyboardStateChange = true;
    });

    glfwSetCharCallback(window, [](GLFWwindow*, u32 codepoint) {
        g_keyboard.set_text_input(core::rune(codepoint));
        g_keyboardStateChange = true;
    });

    // Mouse callbacks

    glfwSetCursorPosCallback(window, [](GLFWwindow*, f64 xpos, f64 ypos) {
        g_mouse.set_pos(xpos, ypos);
        g_mouseStateChange = true;
    });

    glfwSetCursorEnterCallback(window, [](GLFWwindow*, i32 entered) {
        g_mouse.set_in_window(entered != 0);
        g_mouseStateChange = true;
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow*, i32 button, i32 action, i32 mods) {
        // Change keyboard state to take account of the modifiers:
        keyboard_modifiers keyModifiers = keyboard_modifiers::create_from_glfw(mods);
        g_keyboard.set_modifiers(core::move(keyModifiers));
        g_keyboardStateChange = true;
        // Then change the mouse state:
        key_info mouseButton = key_info::create_from_glfw(button, 0, action);
        g_mouse.set_button(mouseButton);
        g_mouseStateChange = true;
    });

    glfwSetScrollCallback(window, [](GLFWwindow*, f64 xoffset, f64 yoffset) {
        g_mouse.set_scroll(xoffset, yoffset);
        g_mouseStateChange = true;
    });

    glfwSetDropCallback(window, [](GLFWwindow*, i32 count, const char** paths) {
        // TODO: save this to the global state.
        std::cout << "Dropped " << count << " files:" << '\n';
        std::cout << "Paths:" << '\n';
        for (i32 i = 0; i < count; ++i) {
            std::cout << paths[i] << std::endl;
        }
        g_mouseStateChange = true;
    });

    // Window callbacks

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, i32 width, i32 height) {
        std::cout << "Window Resize to: " << width << ", " << height << std::endl;
        g_windowStateChange = true;
    });

    glfwSetWindowPosCallback(window, [](GLFWwindow*, i32 xpos, i32 ypos) {
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastWindowDragTime);
        if (diff.count() > 50) {
            // Throttle window drag events to avoid spam rendering.
            std::cout << "Window dragged to: " << xpos << ", " << ypos << std::endl;
            g_lastWindowDragTime = std::chrono::high_resolution_clock::now();
            g_windowStateChange = true;
        }
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        std::cout << "Window close requested" << std::endl;
        glfwSetWindowShouldClose(window, true);
        g_windowStateChange = true;
    });

    glfwSetWindowFocusCallback(window, [](GLFWwindow*, i32 focused) {
        std::cout << "Window " << (focused == 0 ? "unfocused" : "focused") << std::endl;
        g_windowStateChange = true;
    });

    glfwSetWindowMaximizeCallback(window, [](GLFWwindow*, i32 maximized) {
        std::cout << "Window " << (maximized == 0 ? "unmaximized" : "maximized") << std::endl;
        g_windowStateChange = true;
    });

    glfwSetWindowContentScaleCallback(window, [](GLFWwindow*, f32 xscale, f32 yscale) {
        std::cout << "Window content scale: " << xscale << ", " << yscale << std::endl;
        g_windowStateChange = true;
    });

    glfwSetWindowRefreshCallback(window, [](GLFWwindow*) {
        std::cout << "Window refresh requested" << std::endl;
        g_windowStateChange = true;
    });

    while(!glfwWindowShouldClose(window)) {
        // Swap buffers:
        g_render = g_windowStateChange || g_mouseStateChange || g_keyboardStateChange;
        if (g_render) glfwSwapBuffers(window);

        // Clear state:
        g_render = false;
        g_keyboardStateChange = false;
        g_mouseStateChange = false;
        g_windowStateChange = false;
        g_keyboard.clear();
        g_mouse.clear();

        // Poll/Wait events:
        // glfwPollEvents();
        glfwWaitEvents();

        if (g_keyboardStateChange) {
            std::cout << g_keyboard.to_string();
        }
        if (g_mouseStateChange) {
            std::cout << g_mouse.to_string();
        }
    }

    glfwTerminate();
    return 0;
}
