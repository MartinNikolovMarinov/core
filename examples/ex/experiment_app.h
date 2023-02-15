#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include <init_core.h>

#include <grid.h>
#include <keyboard.h>
#include <mouse.h>
#include <shader_prog.h>
#include <shape.h>

#include <chrono>

enum AppExitCodes : i32 {
    APP_EXIT_SUCCESS = 0,
    APP_EXIT_FAILED_TO_INIT = -1,
};

struct AppState {
    // Constants:
    static constexpr Grid2D clipSpaceGrid    = { core::v(-1.0f, -1.0f), core::v(1.0f, 1.0f) };
    static constexpr Grid2D worldSpaceGrid   = { core::v(0.0f, 0.0f), core::v(1000.0f, 1000.0f) };
    static constexpr core::vec2f worldCenter = core::v(worldSpaceGrid.max.x() / 2, worldSpaceGrid.max.y() / 2);

    // Dynamic state:
    Keyboard keyboardState;
    Mouse mouseState;
    bool keyboardStateChange;
    bool mouseStateChange;
    bool windowStateChange;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastWindowDragTime;
    core::vec4<f32> clearColor;
    ShaderProg guiShader;

    // Dynamic GLFW state:
    GLFWwindow* glfwWindow;
    i32 windowWidth;
    i32 windowHeight;
    const char* windowTitle;

    AppState() = default;
    ~AppState();

    void destory();
};

struct App {
    static AppState g_appState;
    static AppState& state();

    ~App();

    i32 init();
    i32 run();
};
