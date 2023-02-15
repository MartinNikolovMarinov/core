#include "experiment_app.h"

#include <fmt/format.h>

AppState::~AppState() { destory(); }

void AppState::destory() { guiShader.destroy(); }

namespace {

GLFWwindow* initGLFWWindow(i32 width, i32 height, const char* title) {
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
    App::state().glfwWindow = window;

    // This sets the modifier bit for caps lock and num lock when a key press event is received.
    // These modifiers need to be locked because there is no need to hold them.
    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    return window;
}

void initGLFWEventHandlers(GLFWwindow* window) {
    // TODO: All these callbacks can return nullptr in case of error. Not checking anything is not great.

    // Error handler

    glfwSetErrorCallback([](i32, const char* description) {
        fmt::print(stderr, "GLFW error: {}\n", description);
    });

    // Keyboard event handlers

    glfwSetKeyCallback(window, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
        App::state().keyboardState.setModifiers(core::move(keyModifiers))
                                  .setKey(core::move(keyInfo));

        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.isPressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        App::state().keyboardStateChange = true;
    });

    glfwSetCharCallback(window, [](GLFWwindow*, u32 codepoint) {
        App::state().keyboardState.setTextInput(core::rune(codepoint));
        App::state().keyboardStateChange = true;
    });

    // Mouse event handlers

    glfwSetCursorPosCallback(window, [](GLFWwindow*, f64 xpos, f64 ypos) {
        App::state().mouseState.setPos(xpos, ypos);
        App::state().mouseStateChange = true;
    });

    glfwSetCursorEnterCallback(window, [](GLFWwindow*, i32 entered) {
        App::state().mouseState.setInWindow(entered != 0);
        App::state().mouseStateChange = true;
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow*, i32 button, i32 action, i32 mods) {
        // Change keyboard state to take account of the modifiers:
        KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        App::state().keyboardState.setModifiers(core::move(keyModifiers));
        App::state().keyboardStateChange = true;
        // Then change the mouse state:
        KeyInfo mouseButton = KeyInfo::createFromGLFW(button, 0, action);
        App::state().mouseState.setButton(mouseButton);
        App::state().mouseStateChange = true;
    });

    glfwSetScrollCallback(window, [](GLFWwindow*, f64 xoffset, f64 yoffset) {
        App::state().mouseState.setScroll(xoffset, yoffset);
        App::state().mouseStateChange = true;
    });

    glfwSetDropCallback(window, [](GLFWwindow*, i32 count, const char** paths) {
        // TODO: save this to the global state.
        fmt::print("Dropped {} files.\n", count);
        fmt::print("Paths:\n");
        for (i32 i = 0; i < count; ++i) {
            fmt::print("  {}\n", paths[i]);
        }
        App::state().mouseStateChange = true;
    });

    // Window event handlers

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, i32 width, i32 height) {
        fmt::print("Window Resize to: {}, {}\n", width, height);
        App::state().windowWidth = width;
        App::state().windowHeight = height;
        glViewport(0, 0, width, height);
        App::state().windowStateChange = true;
    });

    glfwSetWindowPosCallback(window, [](GLFWwindow*, i32 xpos, i32 ypos) {
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - App::state().lastWindowDragTime);
        if (diff.count() > 50) {
            // Throttle window drag events to avoid spam rendering.
            fmt::print("Window dragged to: {}, {}\n", xpos, ypos);
            App::state().lastWindowDragTime = std::chrono::high_resolution_clock::now();
            App::state().windowStateChange = true;
        }
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        fmt::print("Window close requested\n");
        glfwSetWindowShouldClose(window, true);
        App::state().windowStateChange = true;
    });

    glfwSetWindowFocusCallback(window, [](GLFWwindow*, i32 focused) {
        fmt::print("Window focus changed to: {}\n", focused ? "true" : "false");
        App::state().windowStateChange = true;
    });

    glfwSetWindowMaximizeCallback(window, [](GLFWwindow*, i32 maximized) {
        fmt::print("Window maximized changed to: {}\n", maximized ? "true" : "false");
        App::state().windowStateChange = true;
    });

    glfwSetWindowContentScaleCallback(window, [](GLFWwindow*, f32 xscale, f32 yscale) {
        fmt::print("Window content scale: {}, {}\n", xscale, yscale);
        App::state().windowStateChange = true;
    });

    glfwSetWindowRefreshCallback(window, [](GLFWwindow*) {
        fmt::print("Window refresh requested\n");
        App::state().windowStateChange = true;
    });
}

void initOpenGL() {
    glViewport(0, 0, App::state().windowWidth, App::state().windowHeight);

    auto& cc = App::state().clearColor;
    glClearColor(cc.r(), cc.g(), cc.b(), cc.a());

    constexpr bool debugWireFrameMode = true;
    if constexpr (debugWireFrameMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEnable(GL_MULTISAMPLE);
}

ShaderProg createGUIShaderProgram() {
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

bool shouldRender() {
    bool shouldRender = App::state().windowStateChange ||
                        App::state().mouseStateChange  ||
                        App::state().keyboardStateChange;
    return shouldRender;
}

void prepareInputStateForNextFrame() {
    App::state().keyboardStateChange = false;
    App::state().mouseStateChange = false;
    App::state().windowStateChange = false;
    App::state().keyboardState.clear();
    App::state().mouseState.clear();
}

void renderShape(Shape2D& shape) {
    // Bind shape:
    {
        // cache the last bound vbo and vao.
        static u32 lastBoundVboId = core::MAX_U32;
        static u32 lastBoundVaoId = core::MAX_U32;

        if (lastBoundVboId != shape.vboID()) {
            glBindBuffer(GL_ARRAY_BUFFER, shape.vboID());
            lastBoundVboId = shape.vboID();
        }
        if (lastBoundVaoId != shape.vaoID()) {
            glBindVertexArray(shape.vaoID());
            lastBoundVaoId = shape.vaoID();
        }
    }

    Check(App::state().guiShader.setUniform_v("u_color", shape.color()));

    switch (shape.renderMode().mode) {
        case Shape2D::RenderMode::Mode::TRIANGLE_FAN:
            glDrawArrays(GL_TRIANGLE_FAN, 0, shape.vertexCount()); // FIXME: learn more about the TRIANGLE FAN mode! Does it have reasonable performance?
            break;
        case Shape2D::RenderMode::Mode::TRIANGLES:
            glDrawArrays(GL_TRIANGLES, 0, shape.vertexCount());
            break;
        default:
            Assert(false, fmt::format("Unknown render mode: {}", shape.renderMode().mode).c_str());
            return;
    }
}

[[maybe_unused]] void debug__printVertexArr(const core::arr<core::vec2f>& vertices) {
    for(ptr_size i = 0; i < vertices.len(); ++i) {
        const auto& p = vertices[i];
        auto vv = convertVecUsingGrid(p, App::state().worldSpaceGrid, App::state().clipSpaceGrid);
        fmt::print("v_{}: (x:{}, y:{})\n", i, vv.x(), vv.y());
    }
}

}

AppState App::g_appState = {};

AppState& App::state() { return App::g_appState; }

App::~App() {
    glfwTerminate();
    if (App::state().glfwWindow) {
        glfwDestroyWindow(App::state().glfwWindow);
    }
}

i32 App::init() {
    initCore();

    App::state().windowStateChange = true; // Force a render on the first frame.
    App::state().windowWidth = 800;
    App::state().windowHeight = 600;
    App::state().windowTitle = "Hello World";
    App::state().clearColor = core::v(0.2f, 0.3f, 0.3f, 1.0f);

    App::state().glfwWindow = initGLFWWindow(App::state().windowWidth,
                                             App::state().windowHeight,
                                             App::state().windowTitle);
    if (!App::state().glfwWindow) return APP_EXIT_FAILED_TO_INIT;

    initGLFWEventHandlers(App::state().glfwWindow);

    // Init glew after we have a window:
    if (auto err = glewInit(); err != GLEW_OK) {
        const char* errStr = reinterpret_cast<const char*>(glewGetErrorString(err));
        fmt::print(stderr, "Failed to initialize GLEW reason: {}\n", errStr);
        return APP_EXIT_FAILED_TO_INIT;
    }

    initOpenGL();

    App::state().guiShader = createGUIShaderProgram();
    App::state().guiShader.use();
    return APP_EXIT_SUCCESS;
}

i32 App::run() {
    Shape2D::VertexLayout vl;
    vl.stride = sizeof(core::vec2f);
    vl.offset = 0;
    vl.usage = { Shape2D::Usage::Access::STATIC, Shape2D::Usage::AccessType::DRAW };
    vl.posAttribId = ValueOrDie(App::state().guiShader.getAttribLocation("in_pos"));
    vl.renderMode.mode = Shape2D::RenderMode::TRIANGLES;

    Shape2D rectShape = Shape2D::createRect2D(vl, App::state().worldCenter, 500.f, 500.f,
                                              core::v(255u, 0u, 0u, 255u), 1.0f);
    defer { rectShape.destroy(); };

    Shape2D triangleShape = Shape2D::createTriangle2D(vl,
                                                      core::v(200.0f, 200.0f),
                                                      core::v(500.0f, 200.0f),
                                                      core::v(200.0f, 500.0f),
                                                      core::v(0u, 255u, 0u, 255u),
                                                      0.0f);
    defer { triangleShape.destroy(); };

    vl.renderMode.mode = Shape2D::RenderMode::TRIANGLE_FAN;

    Shape2D circleShape = Shape2D::createCircle2D(vl, App::state().worldCenter, 200.0f, 100,
                                                  core::v(255u, 255u, 255u, 255u), 0.0f);
    defer { circleShape.destroy(); };

    core::arr<Shape2D> renderOrder;
    renderOrder.append(core::move(rectShape));
    renderOrder.append(core::move(triangleShape));
    renderOrder.append(core::move(circleShape));

    // TODO: when rendering lines I should use a SDF (Signed distance function)

    Check(App::state().guiShader.setUniform_v("u_worldSpaceGridMin", App::state().worldSpaceGrid.min));
    Check(App::state().guiShader.setUniform_v("u_worldSpaceGridMax", App::state().worldSpaceGrid.max));

    while(!glfwWindowShouldClose(App::state().glfwWindow)) {
        if (shouldRender()) {
            // clear
            glClear(GL_COLOR_BUFFER_BIT);

            // render objects

            // FIXME: DEBUG CODE:
            i32 perfDebugCounter = 1000; // mFIXME: bulk render and measure performance difference.
            while(perfDebugCounter--) {
                // FIXME: can't perform this test until the vertex buffer usage is dynamic.
                // auto direction = core::v(1.0f, 1.0f);
                // for (ptr_size i = 0; i < renderOrder.len(); ++i) {
                //     auto& b = const_cast<core::arr<core::vec2f>&>(renderOrder[i].vertices());
                //     for (ptr_size j = 0; j < b.len(); ++j) {
                //         b[j] += direction;
                //         if (b[j].x() > g_worldSpaceGrid.max.x() || b[j].x() < g_worldSpaceGrid.min.x()) {
                //             direction.x() *= -1.0f;
                //         }
                //         if (b[j].y() > g_worldSpaceGrid.max.y() || b[j].y() < g_worldSpaceGrid.min.y()) {
                //             direction.y() *= -1.0f;
                //         }
                //     }
                // }

                for (ptr_size i = 0; i < renderOrder.len(); ++i) {
                    renderShape(renderOrder[i]);
                }
            }

            // swap buffers
            glfwSwapBuffers(App::state().glfwWindow);
        }

        prepareInputStateForNextFrame();

        // Poll/Wait events:
        // glfwPollEvents();
        glfwWaitEvents();

        if (App::state().keyboardStateChange) {
            fmt::print("{}", App::state().keyboardState.toString());
        }
        if (App::state().mouseStateChange) {
            fmt::print("{}", App::state().mouseState.toString());
        }
    }

    return APP_EXIT_SUCCESS;
}
