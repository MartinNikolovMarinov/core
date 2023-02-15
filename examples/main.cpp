#include <GL/glew.h>
#include <GLFW/glfw3.h>

// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include <init_core.h>

#include <keyboard.h>
#include <shader_prog.h>

#include <chrono>
#include <thread>

using system_clock = std::chrono::system_clock;
template <typename T>
using time_point = std::chrono::time_point<T>;
using high_resolution_clock = std::chrono::high_resolution_clock;

// Global state:
time_point<system_clock> g_lastFrameTime;

i32 main(i32, char const**) {
    initCore();

    // Initialize GLFW window
    GLFWwindow* window;
    {
        if (!glfwInit()) {
            fmt::print(stderr, "Failed to initialize GLFW\n");
            return -1;
        }

        // Hints for OpenGL:
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    #ifdef OS_MAC
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(800, 600, "Hello World", nullptr, nullptr);
        if (!window) {
            fmt::print(stderr, "Failed to create GLFW window\n");
            return -1;
        }
        glfwMakeContextCurrent(window);

        // This sets the modifier bit for caps lock and num lock when a key press event is received.
        // These modifiers need to be locked because there is no need to hold them.
        glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

        // vsync:
        glfwSwapInterval(0); // FIXME: I hate x11 for forcing me to turn off vsync...
    }
    defer {
        glfwTerminate();
        if (window) glfwDestroyWindow(window);
    };

    // Set window events
    {
        glfwSetKeyCallback(window, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
            [[maybe_unused]] KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
            KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
            if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.isPressed()) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        });
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, i32 width, i32 height) {
            fmt::print("Window Resize to: {}, {}\n", width, height);
            glViewport(0, 0, width, height);
        });
    }

    // Init glew after we have a window:
    {
        if (auto err = glewInit(); err != GLEW_OK) {
            const char* errStr = reinterpret_cast<const char*>(glewGetErrorString(err));
            fmt::print(stderr, "Failed to initialize GLEW reason: {}\n", errStr);
            return -1;
        }
    }

    // Create shader program:
    ShaderProg shader;
    {
        const char* vertexGUIShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec3 in_pos;

            uniform vec2 u_worldSpaceGridMin;
            uniform vec2 u_worldSpaceGridMax;

            vec2 convertVecUsingGrid(vec2 src, vec2 fromMin, vec2 fromMax, vec2 toMin, vec2 toMax) {
                vec2 fromRange = fromMax - fromMin;
                vec2 toRange = toMax - toMin;
                vec2 relativeLoc = (src - fromMin) / fromRange;
                vec2 ret = relativeLoc * toRange + toMin;
                return ret;
            }

            void main() {
                const vec2 clipSpaceGridMin = vec2(-1.0, 1.0); // OpenGL flips the y axis.
                const vec2 clipSpaceGridMax = vec2(1.0, -1.0);

                vec2 clipSpacePos = convertVecUsingGrid(in_pos.xy,
                                                        u_worldSpaceGridMin, u_worldSpaceGridMax,
                                                        clipSpaceGridMin, clipSpaceGridMax);

                gl_Position = vec4(clipSpacePos.xy, 1.0, 1.0);
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
        shader = ValueOrDie(ShaderProg::create(vertexGUIShaderSource, fragmentGUIShaderSource));
    }

    // Init global state:
    g_lastFrameTime = high_resolution_clock::now();

    // Main loop:
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

        // Render code:
        glfwSwapBuffers(window);

        // Limit frame-rate to 60fps:
        const auto frameTime = high_resolution_clock::now();
        const auto frameDuration = frameTime - g_lastFrameTime;
        const auto frameDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(frameDuration);
        if (frameDurationMs.count() < (1/60*1000)) {
            std::this_thread::sleep_for(std::chrono::milliseconds((1/60*1000) - frameDurationMs.count()));
        }
        g_lastFrameTime = high_resolution_clock::now();
    }

    return 0;
}
