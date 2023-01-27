#include "keyboard.h"

#include <core.h>

#include <GLFW/glfw3.h>

#include <iostream>

using namespace coretypes;

// GLOBAL STATE:
static keyboard g_keyboard;
static bool g_render;

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

    glfwSetKeyCallback(window, [](GLFWwindow*, i32 key, i32 scancode, i32 action, i32 mods) {
        keyboard_modifiers keyModifiers = keyboard_modifiers::create_from_glfw(mods);
        key_info keyInfo = key_info::create_from_glfw(key, scancode, action);
        g_keyboard.set_key(core::move(keyInfo));
        g_keyboard.set_modifiers(core::move(keyModifiers));
        g_render = true;
    });

    glfwSetCharCallback(window, [](GLFWwindow*, u32 codepoint) {
        g_keyboard.set_text_input(core::rune(codepoint));
        g_render = true;
    });

    while(!glfwWindowShouldClose(window)) {
        g_render = false;
        glfwSwapBuffers(window);
        // glfwPollEvents();
        glfwWaitEvents();

        if (g_render) {
            std::cout << g_keyboard.to_string() << std::endl;
        }
    }

    glfwTerminate();
    return 0;
}
