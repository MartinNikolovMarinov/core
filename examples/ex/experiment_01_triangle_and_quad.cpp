#include "experiment_01_triangle_and_quad.h"

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

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        [[maybe_unused]] KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.isPressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW KeyCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow*, i32 width, i32 height) {
        glViewport(0, 0, width, height);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW FramebufferSizeCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    // Set Error callback:
    glfwSetErrorCallback([](i32 errCode, const char* errDesc) {
        fmt::print(stderr, "GLFW error: {}, reason: {}\n", errCode, errDesc ? errDesc : "Unknown");
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW Error callback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    return {};
}

void destroy() {
    State& g_s = state();
    glDeleteBuffers(1, &g_s.trinagleVBOId);
    glDeleteVertexArrays(1, &g_s.triangleVAOId);
    glDeleteBuffers(1, &g_s.quadVBOId);
    glDeleteVertexArrays(1, &g_s.quadVAOId);
    glDeleteBuffers(1, &g_s.quadEBOId);
    g_s.shaderProg.destroy();
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create shader program:
    {
        const char* vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 in_pos;

            void main() {
                gl_Position = vec4(in_pos.xyz, 1.0);
            }
        )";
        const char* fragShaderSource = R"(
            #version 330 core

            out vec4 out_fragColor;
            uniform vec4 u_color;

            void main() {
                out_fragColor = u_color;
            }
        )";
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexShaderSource, fragShaderSource));
    }

    // Create triangle:
    {
        g_s.triangleColor = core::v(1.0f, 0.5f, 0.2f, 1.0f);

        core::arr<core::vec2f> vertices(0, 3);
        vertices.append({ -0.5f, -0.5f }); // left
        vertices.append({ 0.5f, -0.5f });  // right
        vertices.append({ 0.0f, 0.5f });   // top

        for (i32 i = 0; i < vertices.len(); ++i) {
            vertices[i] = core::scale(vertices[i], core::v(0.0f, 0.0f), core::v(0.5f, 0.5f));
            vertices[i] = core::translate(vertices[i], core::v(0.5f, 0.0f));
        }

        // Create VBO, bind it and store the vertex buffer data:
        glGenBuffers(1, &g_s.trinagleVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.trinagleVBOId);
        ptr_size bufferDataSize = vertices.len() * sizeof(core::vec2f);
        glBufferData(GL_ARRAY_BUFFER, bufferDataSize, vertices.data(), GL_STATIC_DRAW);

        // Create VAO and bind it:
        glGenVertexArrays(1, &g_s.triangleVAOId);
        glBindVertexArray(g_s.triangleVAOId);

        // Link vertex attributes:
        constexpr ptr_size stride = sizeof(core::vec2f);
        constexpr ptr_size dimensions = core::vec2f::dimensions();
        constexpr u32 inPosAttribLocation = 0;
        glVertexAttribPointer(inPosAttribLocation, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
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

        for (i32 i = 0; i < vertices.len(); ++i) {
            vertices[i] = core::scale(vertices[i], core::v(0.0f, 0.0f), core::v(0.5f, 0.5f));
            vertices[i] = core::translate(vertices[i], core::v(-0.5f, 0.0f));
        }

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
        constexpr ptr_size dimensions = core::vec2f::dimensions();
        constexpr u32 inPosAttribLocation = 0;
        glVertexAttribPointer(inPosAttribLocation, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(inPosAttribLocation);
    }

    return {};
}

void mainLoop(CommonState& commonState) {
    State& g_s = state();

    // Debug reset binds to make sure everyting is bound correctly later on.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

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

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);

    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
}

} // namespace triangle_and_quad_ex_01
