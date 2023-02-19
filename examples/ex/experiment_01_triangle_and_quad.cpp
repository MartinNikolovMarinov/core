#include "experiment_01_triangle_and_quad.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>

#include <keyboard.h>
#include <shader_prog.h>

namespace triangle_and_quad_ex_01 {

namespace {

struct State {
    ShaderProg shaderProg;

    u32 trinagleVBOId;
    u32 triangleVAOId;
    core::vec4f triangleColor;

    u32 quadVBOId;
    u32 quadVAOId;
    u32 quadEBOId;
    core::vec4f quadColor;
};

State& state() {
    static State g_state = {};
    return g_state;
}

} // namespace

i32 init(AppState &s) {
    GLFWwindow* glfwWindow = reinterpret_cast<GLFWwindow*>(getNativeWindowHandle(s.mainWindow));
    if (!glfwWindow) {
        fmt::print(stderr, "Failed to get GLFW window handle\n");
        return AppExitCodes::APP_EXIT_FAILED_TO_INIT;
    }

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        [[maybe_unused]] KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.isPressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });

    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow*, i32 width, i32 height) {
        glViewport(0, 0, width, height);
    });

    return AppExitCodes::APP_EXIT_SUCCESS;
}

void destroy() {}

void preMainLoop(AppState&) {
    State& g_s = state();

    // Create shader program:
    {
        const char* vertexGUIShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 in_pos;

            void main() {
                gl_Position = vec4(in_pos.xyz, 1.0);
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
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexGUIShaderSource, fragmentGUIShaderSource));
    }

    // Create triangle:
    {
        g_s.triangleColor = core::v(1.0f, 0.5f, 0.2f, 1.0f);

        core::arr<core::vec2f> vertices(0, 3);
        vertices.append({ -0.5f, -0.5f }); // left
        vertices.append({ 0.5f, -0.5f });  // right
        vertices.append({ 0.0f, 0.5f });   // top

        core::scale(vertices, core::v(0.0f, 0.0f), core::v(0.5f, 0.5f));
        core::translate(vertices, core::v(0.5f, 0.0f));

        // Create VBO, bind it and store the vertex buffer data:
        glGenBuffers(1, &g_s.trinagleVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.trinagleVBOId);
        ptr_size bufferDataSize = vertices.len() * sizeof(core::vec2f);
        glBufferData(GL_ARRAY_BUFFER, bufferDataSize, vertices.data(), GL_STATIC_DRAW);

        // Create VAO and bind it:
        glGenVertexArrays(1, &g_s.triangleVAOId);
        glBindVertexArray(g_s.triangleVAOId);
        constexpr ptr_size stride = sizeof(core::vec2f);
        constexpr ptr_size dimentions = core::vec2f::dimentions();
        constexpr u32 inPosAttribLocation = 0;

        // Link vertex attributes:
        glVertexAttribPointer(inPosAttribLocation, dimentions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(inPosAttribLocation);
    }

    // Create rectangle:
    {
        g_s.quadColor = core::v(0.2f, 0.3f, 0.8f, 1.0f);

        core::arr<core::vec2f> vertices(0, 4);
        vertices.append({ 0.5f, 0.5f });   // left
        vertices.append({ 0.5f, -0.5f });  // right
        vertices.append({ -0.5f, -0.5f }); // top
        vertices.append({ -0.5f, 0.5f });  // bottom

        core::scale(vertices, core::v(0.0f, 0.0f), core::v(0.5f, 0.5f));
        core::translate(vertices, core::v(-0.5f, 0.0f));

        core::arr<u32> indices(0, 6);
        // First triangle:
        indices.append(0);
        indices.append(1);
        indices.append(3);
        // Second triangle:
        indices.append(1);
        indices.append(2);
        indices.append(3);

         // Create VBO, bind it and store the vertex buffer data:
        glGenBuffers(1, &g_s.quadVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBOId);
        ptr_size bufferDataSize = vertices.len() * sizeof(core::vec2f);
        glBufferData(GL_ARRAY_BUFFER, bufferDataSize, vertices.data(), GL_STATIC_DRAW);

        // Create VAO and bind it:
        glGenVertexArrays(1, &g_s.quadVAOId);
        glBindVertexArray(g_s.quadVAOId);

        // Create EBO and bind it and store the index buffer data:
        glGenBuffers(1, &g_s.quadEBOId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBOId);
        ptr_size indicesBufferDataSize = indices.len() * sizeof(u32);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesBufferDataSize, indices.data(), GL_STATIC_DRAW);

         // Link vertex attributes:
        constexpr ptr_size stride = sizeof(core::vec2f);
        constexpr ptr_size dimentions = core::vec2f::dimentions();
        constexpr u32 inPosAttribLocation = 0;
        glVertexAttribPointer(inPosAttribLocation, dimentions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(inPosAttribLocation);
    }
}

void mainLoop(AppState&) {
    // Debug reset binds to make sure everyting is bound correctly later on.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    State& g_s = state();

    g_s.shaderProg.use();
    g_s.shaderProg.setUniform_v("u_color", g_s.triangleColor);

    glBindVertexArray(g_s.triangleVAOId);
    glBindBuffer(GL_ARRAY_BUFFER, g_s.trinagleVBOId);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    g_s.shaderProg.setUniform_v("u_color", g_s.quadColor);

    glBindVertexArray(g_s.quadVAOId);
    glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBOId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBOId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

} // namespace triangle_and_quad_ex_01
