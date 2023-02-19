#include "common.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>

namespace common {

struct Window {
    GLFWwindow* glfwWindow;
    i32 width;
    i32 height;
    const char* title;
};

void* getNativeWindowHandle(Window* w) {
    if (w) return w->glfwWindow;
    return nullptr;
}

AppState& state() {
    static AppState g_state = {};
    return g_state;
}

namespace {

i32 initGlfw() {
    if (!glfwInit()) {
        fmt::print(stderr, "Failed to initialize GLFW\n");
        return AppExitCodes::APP_EXIT_FAILED_TO_INIT;
    }

    // Setup GLFW window hints:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#ifdef OS_MAC
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);

    return AppExitCodes::APP_EXIT_SUCCESS;
}

i32 initGlew() {
    if (auto err = glewInit(); err != GLEW_OK) {
        const char* errStr = reinterpret_cast<const char*>(glewGetErrorString(err));
        fmt::print(stderr, "Failed to initialize GLEW reason: {}\n", errStr);
        return AppExitCodes::APP_EXIT_FAILED_TO_INIT;
    }
    return AppExitCodes::APP_EXIT_SUCCESS;
}

void initOpenGL() {
    AppState& g_s = state();
    glViewport(0, 0, g_s.mainWindow->width, g_s.mainWindow->height);

    auto& cc = g_s.clearColor;
    glClearColor(cc.r(), cc.g(), cc.b(), cc.a());

    if (g_s.debugWireFrameMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // TODO: Might want to disable this if I want to do implement anti-alising myself.
    //       Or if I deciede to render to a backbuffer/texture. Leave it for the examples for now.
    // Enable building anti-aliasing:
    glEnable(GL_MULTISAMPLE);
}

}

i32 init(InitProps&& props) {
    Assert(props.mainLoopCb != nullptr, "Provided nullptr for Main Loop.");

    initCore();
    if (i32 err = initGlfw(); err != AppExitCodes::APP_EXIT_SUCCESS) return err;

    AppState& g_s = state();
    g_s.mainLoopCb = props.mainLoopCb;
    g_s.preMainLoopCb = props.preMainLoopCb;
    g_s.debugWireFrameMode = props.debugWireFrameMode;
    g_s.clearColor = props.clearColor;
    g_s.firstFrameTimestamp_ms = 0;
    g_s.frameCount = 0;

    // Create the glfw main window:
    g_s.mainWindow = reinterpret_cast<Window*>(core::alloc<CORE_DEFAULT_ALLOCATOR()>(sizeof(Window)));
    g_s.mainWindow->width = props.width;
    g_s.mainWindow->height = props.height;
    g_s.mainWindow->title = props.title;
    g_s.mainWindow->glfwWindow = glfwCreateWindow(g_s.mainWindow->width, g_s.mainWindow->height, g_s.mainWindow->title, 0, 0);
    if (!g_s.mainWindow->glfwWindow) {
        fmt::print(stderr, "Failed to create GLFW window\n");
        return APP_EXIT_FAILED_TO_INIT;
    }

    glfwMakeContextCurrent(g_s.mainWindow->glfwWindow);
    // This sets the modifier bit for caps lock and num lock when a key press event is received.
    // These modifiers need to be locked because there is no need to hold them:
    glfwSetInputMode(g_s.mainWindow->glfwWindow, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    // Enable vsync:
    glfwSwapInterval(1);

    // Init glew after we have a window:
    if (i32 err = initGlew(); err != AppExitCodes::APP_EXIT_SUCCESS) return err;

    // Initialize OpenGL:
    initOpenGL();

    // Call the custom user provided callback:
    if (props.initStateCb) {
        if (i32 err = props.initStateCb(g_s); err != AppExitCodes::APP_EXIT_SUCCESS) {
            return err;
        }
    }

    return AppExitCodes::APP_EXIT_SUCCESS;
}

void destroy() {
    AppState& g_s = state();
    glfwTerminate();
    if (g_s.mainWindow) {
        glfwDestroyWindow(g_s.mainWindow->glfwWindow);
        core::free<CORE_DEFAULT_ALLOCATOR()>(g_s.mainWindow);
    }
}

i32 run() {
    AppState& g_s = state();

    g_s.firstFrameTimestamp_ms = ValueOrDie(core::os_unix_time_stamp_in_ms());
    if (g_s.preMainLoopCb) g_s.preMainLoopCb(g_s);

    while (!glfwWindowShouldClose(g_s.mainWindow->glfwWindow)) {
        u64 elapsedTime_ms = ValueOrDie(core::os_unix_time_stamp_in_ms()) - g_s.firstFrameTimestamp_ms;
        g_s.frameCount++;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        g_s.mainLoopCb(g_s);
        glfwSwapBuffers(g_s.mainWindow->glfwWindow);

        if (elapsedTime_ms) {
            double elapsedSeconds = elapsedTime_ms / 1000.0;
            constexpr double epsilon = 0.000000001;
            if (core::safe_eq(elapsedSeconds, 0.0, epsilon)) continue;
            double fps = g_s.frameCount / elapsedSeconds;
            fmt::print("Frame: {}, FPS: {:f}\n", g_s.frameCount, fps);
        }
    }

    return AppExitCodes::APP_EXIT_SUCCESS;
}

} // namespace common
