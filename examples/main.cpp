#include "unity_build.h"

#include <iostream>
#include <chrono>

struct Mesh {
    struct Usage {
        enum Access : u32 {
            STATIC = 0,
            DYNAMIC = 1,
            STREAM = 2,
        };
        enum AccessType : u32 {
            DRAW = 0,
            READ = 1,
            COPY = 2,
        };

        Access a;
        AccessType t;

        u32 usage_Gl() const {
            u32 ret = -1;
            if (a == Access::STATIC) {
                if (t == AccessType::DRAW) ret = GL_STATIC_DRAW;
                if (t == AccessType::READ) ret = GL_STATIC_READ;
                if (t == AccessType::COPY) ret = GL_STATIC_COPY;
            }
            if (a == Access::DYNAMIC) {
                if (t == AccessType::DRAW) ret = GL_DYNAMIC_DRAW;
                if (t == AccessType::READ) ret = GL_DYNAMIC_READ;
                if (t == AccessType::COPY) ret = GL_DYNAMIC_COPY;
            }
            if (a == Access::STREAM) {
                if (t == AccessType::DRAW) ret = GL_STREAM_DRAW;
                if (t == AccessType::READ) ret = GL_STREAM_READ;
                if (t == AccessType::COPY) ret = GL_STREAM_COPY;
            }
            return ret;
        }
    };

    struct VertexLayout {
        u32 stride;
        u32 offset;
        Usage usage;
    };

    static Mesh create(const Mesh::VertexLayout& vl, core::arr<core::vec2f>&& vertices) {
        Mesh m;
        m.m_vertices = core::move(vertices);

        u32 vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        auto bufferDataSize = m.m_vertices.len() * sizeof(core::vec2f);
        glBufferData(GL_ARRAY_BUFFER, bufferDataSize, m.m_vertices.data(), vl.usage.usage_Gl());
        return m;
    }

    void destroy() const {
        glDeleteBuffers(1, &m_vboId);
    }

    i32 vertex_count() const {
        return m_vertices.len();
    }

    void bind() const {
        static u32 lastBoundVboId = 0; // cash the last bound vbo.
        if (lastBoundVboId == m_vboId) return;
        glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
        lastBoundVboId = m_vboId;
    }

private:
    // TODO: Do I actually need to store indices in the mesh when I will only render in 2d?
    //       Is this struct a mesh if it has not indices or normals? Kinda looks like a vertex buffer.
    //       Should I even be storing the vertices in this struct, or should I just store the vbo id?
    core::arr<core::vec2f> m_vertices;
    u32 m_vboId;
};

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
    GLFWwindow* glfwWindow;
    i32 windowWidth;
    i32 windowHeight;
    const char* windowTitle;
};
static AppState g_appState;

GLFWwindow* init_glfw_window(i32 width, i32 height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Hints for OpenGL:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    // Store some of the window state in the global state:
    g_appState.glfwWindow = window;
    g_appState.windowWidth = width;
    g_appState.windowHeight = height;
    g_appState.windowTitle = title;

    return window;
}

void init_glfw_event_handlers(GLFWwindow* window) {
    // TODO: All these callbacks can return nullptr in case of error. Not checking anything is not great.

    // Error handler

    glfwSetErrorCallback([](i32, const char* description) {
        std::cerr << "GLFW Error: " << description << std::endl;
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
        std::cout << "Dropped " << count << " files:" << '\n';
        std::cout << "Paths:" << '\n';
        for (i32 i = 0; i < count; ++i) {
            std::cout << paths[i] << std::endl;
        }
        g_appState.mouseStateChange = true;
    });

    // Window event handlers

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, i32 width, i32 height) {
        std::cout << "Window Resize to: " << width << ", " << height << std::endl;
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
            std::cout << "Window dragged to: " << xpos << ", " << ypos << std::endl;
            g_appState.lastWindowDragTime = std::chrono::high_resolution_clock::now();
            g_appState.windowStateChange = true;
        }
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        std::cout << "Window close requested" << std::endl;
        glfwSetWindowShouldClose(window, true);
        g_appState.windowStateChange = true;
    });

    glfwSetWindowFocusCallback(window, [](GLFWwindow*, i32 focused) {
        std::cout << "Window " << (focused == 0 ? "unfocused" : "focused") << std::endl;
        g_appState.windowStateChange = true;
    });

    glfwSetWindowMaximizeCallback(window, [](GLFWwindow*, i32 maximized) {
        std::cout << "Window " << (maximized == 0 ? "unmaximized" : "maximized") << std::endl;
        g_appState.windowStateChange = true;
    });

    glfwSetWindowContentScaleCallback(window, [](GLFWwindow*, f32 xscale, f32 yscale) {
        std::cout << "Window content scale: " << xscale << ", " << yscale << std::endl;
        g_appState.windowStateChange = true;
    });

    glfwSetWindowRefreshCallback(window, [](GLFWwindow*) {
        std::cout << "Window refresh requested" << std::endl;
        g_appState.windowStateChange = true;
    });
}

void init_openGL() {
    glViewport(0, 0, g_appState.windowWidth, g_appState.windowHeight);
}

bool should_render() {
    bool shouldRender = g_appState.windowStateChange ||
                        g_appState.mouseStateChange ||
                        g_appState.keyboardStateChange;
    return shouldRender;
}

void prepare_state_for_next_frame() {
    g_appState.keyboardStateChange = false;
    g_appState.mouseStateChange = false;
    g_appState.windowStateChange = false;
    g_appState.keyboardState.clear();
    g_appState.mouseState.clear();

    // Clear the screen:
    auto& cc = g_appState.clearColor;
    glClearColor(cc.r(), cc.g(), cc.b(), cc.a()); // TODO: don't set this on every frame.
    glClear(GL_COLOR_BUFFER_BIT);
}

i32 main() {
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
        std::cerr << "Failed to initialize GLEW reason: " << glewGetErrorString(err) << std::endl;
        return APP_EXIT_FAILED_TO_INIT;
    }

    // Create opengl program:
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        void main() {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        void main() {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";
    ShaderProg program = ValueOrDie(ShaderProg::create(vertexShaderSource, fragmentShaderSource));

    // TODO: Vertices are in clip space coordinates, aka. normalized device coordinates (NDC).
    //       Vertex points should be stored in UI space or whatever space the UI is in.
    core::arr<core::vec2f> exampleTriangleVertices(0, 3);
    exampleTriangleVertices.append(core::v(-0.5f, -0.5f))
                           .append(core::v( 0.5f, -0.5f))
                           .append(core::v( 0.0f,  0.5f));
    Mesh::Usage usage = { Mesh::Usage::Access::STATIC, Mesh::Usage::AccessType::DRAW };
    Mesh::VertexLayout vl = { 0, 0, usage };
    auto mesh = Mesh::create(core::move(vl), core::move(exampleTriangleVertices));
    mesh.bind();

    // Linking vertex attributes:
    constexpr ptr_size stride = sizeof(core::vec2f);
    glVertexAttribPointer(0, core::vec2f::dimmentions(), GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    while(!glfwWindowShouldClose(window)) {
        prepare_state_for_next_frame();

        // TODO: temporary comment out the should render check.
        // if (should_render()) {
        program.use();
        mesh.bind();
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_count());
        // }

        glfwSwapBuffers(window);

        // Poll/Wait events:
        // glfwPollEvents();
        glfwWaitEvents();

        if (g_appState.keyboardStateChange) {
            std::cout << g_appState.keyboardState.to_string();
        }
        if (g_appState.mouseStateChange) {
            std::cout << g_appState.mouseState.to_string();
        }
    }

    return APP_EXIT_SUCCESS;
}
