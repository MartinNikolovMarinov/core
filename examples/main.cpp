#include <GL/glew.h>
#include <GLFW/glfw3.h>

// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include <init_core.h>

#include <keyboard.h>
#include <shader_prog.h>

// TODO: Drawing line segments is quite the problem to solve in one pass. I first need to:
//        - Go through all tutorials on https://learnopengl.com/ and make sure I understand everything.
//        - Leave some good working examples with as little abstraciton as possible, without repeating code too much.
//        - Examples need to be easy to switch between and test new things.
//        - Read very carefully about instanced rendering in OpenGL from https://learnopengl.com/Advanced-OpenGL/Instancing
//        - Then trying to slowly apply this to the examples here - https://wwwtyro.net/2019/11/18/instanced-lines.html
//        - Maybe explore more techniques for rendering many line segments.
//          Although this should be good enough for most applications. Especially for my needs.

// Global state:
u64 g_firstFrameTimestamp_ms;
u64 g_frameCount;

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

        // Enable vsync:
        glfwSwapInterval(1);
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
    g_firstFrameTimestamp_ms = ValueOrDie(core::os_unix_time_stamp_in_ms());
    g_frameCount = 0;

    // Main loop:
    while(!glfwWindowShouldClose(window)) {
        u64 elapsedTime_ms = ValueOrDie(core::os_unix_time_stamp_in_ms()) - g_firstFrameTimestamp_ms;
        g_frameCount++;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        // Render code:
        glfwSwapBuffers(window);

        if (elapsedTime_ms) {
            double elapsedSeconds = elapsedTime_ms / 1000.0;
            constexpr double epsilon = 0.000000001;
            if (core::safe_eq(elapsedSeconds, 0.0, epsilon)) continue;
            double fps = g_frameCount / elapsedSeconds;
            fmt::print("Frame: {}, FPS: {:f}\n", g_frameCount, fps);
        }
    }

    return 0;
}
