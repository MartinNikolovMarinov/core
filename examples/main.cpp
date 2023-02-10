#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fmt/format.h>

// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include "init_core.h"

#include "grid.h"
#include "keyboard.h"
#include "mouse.h"
#include "shader_prog.h"
#include "shape.h"

#include <chrono>

enum app_exit_codes : i32 {
    APP_EXIT_SUCCESS = 0,
    APP_EXIT_FAILED_TO_INIT = -1,
};

struct AppState {
    keyboard keyboardState;
    mouse mouseState;
    bool keyboardStateChange;
    bool mouseStateChange;
    bool windowStateChange;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastWindowDragTime;
    core::vec4<f32> clearColor;
    ShaderProg guiShader;

    GLFWwindow* glfwWindow;
    i32 windowWidth;
    i32 windowHeight;
    const char* windowTitle;

    AppState() = default;
    ~AppState() { destory(); }

    void destory() {
        guiShader.destroy();
    }
};
static AppState g_appState;

static constexpr Grid2D g_clipSpaceGrid  = { core::v(-1.0f, -1.0f), core::v(1.0f, 1.0f) };
static constexpr Grid2D g_worldSpaceGrid = { core::v(0.0f, 0.0f), core::v(1000.0f, 1000.0f) };

GLFWwindow* init_glfw_window(i32 width, i32 height, const char* title) {
    if (!glfwInit()) {
        fmt::print(stderr, "Failed to initialize GLFW\n");
        return nullptr;
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

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        fmt::print(stderr, "Failed to create GLFW window\n");
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    g_appState.glfwWindow = window;

    // This sets the modifier bit for caps lock and num lock when a key press event is received.
    // These modifiers need to be locked because there is no need to hold them.
    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    return window;
}

void init_glfw_event_handlers(GLFWwindow* window) {
    // TODO: All these callbacks can return nullptr in case of error. Not checking anything is not great.

    // Error handler

    glfwSetErrorCallback([](i32, const char* description) {
        fmt::print(stderr, "GLFW error: {}\n", description);
    });

    // Keyboard event handlers

    glfwSetKeyCallback(window, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        keyboard_modifiers keyModifiers = keyboard_modifiers::create_from_glfw(mods);
        key_info keyInfo = key_info::create_from_glfw(key, scancode, action);
        g_appState.keyboardState.set_modifiers(core::move(keyModifiers))
                  .set_key(core::move(keyInfo));

        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.is_pressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        g_appState.keyboardStateChange = true;
    });

    glfwSetCharCallback(window, [](GLFWwindow*, u32 codepoint) {
        g_appState.keyboardState.set_text_input(core::rune(codepoint));
        g_appState.keyboardStateChange = true;
    });

    // Mouse event handlers

    glfwSetCursorPosCallback(window, [](GLFWwindow*, f64 xpos, f64 ypos) {
        g_appState.mouseState.set_pos(xpos, ypos);
        g_appState.mouseStateChange = true;
    });

    glfwSetCursorEnterCallback(window, [](GLFWwindow*, i32 entered) {
        g_appState.mouseState.set_in_window(entered != 0);
        g_appState.mouseStateChange = true;
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow*, i32 button, i32 action, i32 mods) {
        // Change keyboard state to take account of the modifiers:
        keyboard_modifiers keyModifiers = keyboard_modifiers::create_from_glfw(mods);
        g_appState.keyboardState.set_modifiers(core::move(keyModifiers));
        g_appState.keyboardStateChange = true;
        // Then change the mouse state:
        key_info mouseButton = key_info::create_from_glfw(button, 0, action);
        g_appState.mouseState.set_button(mouseButton);
        g_appState.mouseStateChange = true;
    });

    glfwSetScrollCallback(window, [](GLFWwindow*, f64 xoffset, f64 yoffset) {
        g_appState.mouseState.set_scroll(xoffset, yoffset);
        g_appState.mouseStateChange = true;
    });

    glfwSetDropCallback(window, [](GLFWwindow*, i32 count, const char** paths) {
        // TODO: save this to the global state.
        fmt::print("Dropped {} files.\n", count);
        fmt::print("Paths:\n");
        for (i32 i = 0; i < count; ++i) {
            fmt::print("  {}\n", paths[i]);
        }
        g_appState.mouseStateChange = true;
    });

    // Window event handlers

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, i32 width, i32 height) {
        fmt::print("Window Resize to: {}, {}\n", width, height);
        g_appState.windowWidth = width;
        g_appState.windowHeight = height;
        glViewport(0, 0, width, height);
        g_appState.windowStateChange = true;
    });

    glfwSetWindowPosCallback(window, [](GLFWwindow*, i32 xpos, i32 ypos) {
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_appState.lastWindowDragTime);
        if (diff.count() > 50) {
            // Throttle window drag events to avoid spam rendering.
            fmt::print("Window dragged to: {}, {}\n", xpos, ypos);
            g_appState.lastWindowDragTime = std::chrono::high_resolution_clock::now();
            g_appState.windowStateChange = true;
        }
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        fmt::print("Window close requested\n");
        glfwSetWindowShouldClose(window, true);
        g_appState.windowStateChange = true;
    });

    glfwSetWindowFocusCallback(window, [](GLFWwindow*, i32 focused) {
        fmt::print("Window focus changed to: {}\n", focused ? "true" : "false");
        g_appState.windowStateChange = true;
    });

    glfwSetWindowMaximizeCallback(window, [](GLFWwindow*, i32 maximized) {
        fmt::print("Window maximized changed to: {}\n", maximized ? "true" : "false");
        g_appState.windowStateChange = true;
    });

    glfwSetWindowContentScaleCallback(window, [](GLFWwindow*, f32 xscale, f32 yscale) {
        fmt::print("Window content scale: {}, {}\n", xscale, yscale);
        g_appState.windowStateChange = true;
    });

    glfwSetWindowRefreshCallback(window, [](GLFWwindow*) {
        fmt::print("Window refresh requested\n");
        g_appState.windowStateChange = true;
    });
}

void init_openGL() {
    glViewport(0, 0, g_appState.windowWidth, g_appState.windowHeight);

    auto& cc = g_appState.clearColor;
    glClearColor(cc.r(), cc.g(), cc.b(), cc.a());

    constexpr bool debugWireFrameMode = false;
    if constexpr (debugWireFrameMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEnable(GL_MULTISAMPLE);
}

ShaderProg create_gui_shader_program() {
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
    ShaderProg guiShader = ValueOrDie(ShaderProg::create(vertexGUIShaderSource, fragmentGUIShaderSource));
    return guiShader;
}

bool should_render() {
    bool shouldRender = g_appState.windowStateChange ||
                        g_appState.mouseStateChange ||
                        g_appState.keyboardStateChange;
    return shouldRender;
}

void prepare_input_state_for_next_frame() {
    g_appState.keyboardStateChange = false;
    g_appState.mouseStateChange = false;
    g_appState.windowStateChange = false;
    g_appState.keyboardState.clear();
    g_appState.mouseState.clear();
}

void render_shape(Shape2D& shape) {
    // Bind shape:
    {
        // cache the last bound vbo and vao.
        static u32 lastBoundVboId = core::MAX_U32;
        static u32 lastBoundVaoId = core::MAX_U32;

        if (lastBoundVboId != shape.vbo_id()) {
            glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_id());
            lastBoundVboId = shape.vbo_id();
        }
        if (lastBoundVaoId != shape.vao_id()) {
            glBindVertexArray(shape.vao_id());
            lastBoundVaoId = shape.vao_id();
        }
    }

    Check(g_appState.guiShader.set_uniform_v("u_color", shape.fill_color()));
    glDrawArrays(GL_TRIANGLES, 0, shape.vertex_count());
}

Shape2D create_rectangle() {
    core::arr<core::vec2f> rectVerts(0, 6);
    rectVerts.append(core::v(250.0f, 750.0f))
             .append(core::v(250.0f, 250.0f))
             .append(core::v(750.0f, 750.0f))
             .append(core::v(750.0f, 750.0f))
             .append(core::v(250.0f, 250.0f))
             .append(core::v(750.0f, 250.0f));

    // FIXME: TMP rotation testing code:
    // core::rotate_right(rectVerts, core::v(500.0f, 500.0f), core::deg_to_rad(5.0f));
    core::rotate(rectVerts, core::v(500.0f, 500.0f), core::deg_to_rad(5.0f));

    Shape2D::VertexLayout vl;
    vl.stride = sizeof(core::vec2f);
    vl.offset = 0;
    vl.usage = { Shape2D::Usage::Access::STATIC, Shape2D::Usage::AccessType::DRAW };
    vl.posAttribId = ValueOrDie(g_appState.guiShader.get_attrib_location("in_pos"));
    Shape2D rectShape = Shape2D::create(vl, core::v(1.0f, 0.0f, 0.0f, 1.0f), 1000.f, core::move(rectVerts));
    return rectShape;
}

Shape2D create_triangle() {
    core::arr<core::vec2f> triangleVerts(0, 3);
    triangleVerts.append(core::v(500.0f, 0.0f))
                 .append(core::v(0.0f, 1000.0f))
                 .append(core::v(1000.0f, 1000.0f));

    // FIXME: TMP rotation testing code:
    // core::rotate_right(triangleVerts, core::v(500.0f, 500.0f), core::deg_to_rad(30.0f));

    Shape2D::VertexLayout vl;
    vl.stride = sizeof(core::vec2f);
    vl.offset = 0;
    vl.usage = { Shape2D::Usage::Access::STATIC, Shape2D::Usage::AccessType::DRAW };
    vl.posAttribId = ValueOrDie(g_appState.guiShader.get_attrib_location("in_pos"));
    Shape2D triangleShape = Shape2D::create(vl, core::v(0.0f, 0.0f, 1.0f, 1.0f), 999.f, core::move(triangleVerts));
    return triangleShape;
}

i32 main(i32, char const**) {
    init_core();

    g_appState = {}; // Zero out the global state.
    g_appState.windowStateChange = true; // Force a render on the first frame.
    g_appState.windowWidth = 800;
    g_appState.windowHeight = 600;
    g_appState.windowTitle = "Hello World";
    g_appState.clearColor = core::v(0.2f, 0.3f, 0.3f, 1.0f);

    GLFWwindow* window = init_glfw_window(g_appState.windowWidth, g_appState.windowHeight, g_appState.windowTitle);
    defer { glfwTerminate(); };
    if (!window) return APP_EXIT_FAILED_TO_INIT;
    defer { glfwDestroyWindow(window); };

    init_glfw_event_handlers(window);

    // Init glew after we have a window:
    if (auto err = glewInit(); err != GLEW_OK) {
        const char* errStr = reinterpret_cast<const char*>(glewGetErrorString(err));
        fmt::print(stderr, "Failed to initialize GLEW reason: {}\n", errStr);
        return APP_EXIT_FAILED_TO_INIT;
    }

    init_openGL();

    g_appState.guiShader = create_gui_shader_program();
    g_appState.guiShader.use();

    Shape2D rectShape = create_rectangle();
    defer { rectShape.destroy(); };

    Shape2D triangleShape = create_triangle();
    defer { triangleShape.destroy(); };

    while(!glfwWindowShouldClose(window)) {
        if (should_render()) {
            // clear
            glClear(GL_COLOR_BUFFER_BIT);
            // render objects
            Check(g_appState.guiShader.set_uniform_v("u_worldSpaceGridMin", g_worldSpaceGrid.min));
            Check(g_appState.guiShader.set_uniform_v("u_worldSpaceGridMax", g_worldSpaceGrid.max));
            // TODO: implement z index ordering for rendering every shape. Shapes probably need to exist in the global state or whaterver.
            if (rectShape.z_index() < triangleShape.z_index()) {
                render_shape(rectShape);
                render_shape(triangleShape);
            }
            else {
                render_shape(triangleShape);
                render_shape(rectShape);
            }

            // swap buffers
            glfwSwapBuffers(window);
        }

        prepare_input_state_for_next_frame();

        // Poll/Wait events:
        // glfwPollEvents();
        glfwWaitEvents();

        if (g_appState.keyboardStateChange) {
            fmt::print("{}", g_appState.keyboardState.to_string());
        }
        if (g_appState.mouseStateChange) {
            fmt::print("{}", g_appState.mouseState.to_string());
        }
    }

    return APP_EXIT_SUCCESS;
}
