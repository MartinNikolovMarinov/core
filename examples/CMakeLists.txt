cmake_minimum_required(VERSION 3.2)

project(main)

# Global CMake options:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Create executable:
add_executable(${PROJECT_NAME}
    main.cpp

    # Examples:
    ex/experiment_app.cpp
    ex/experiment_app.h
    ex/experiment_01_triangle_and_quad.h
    ex/experiment_01_triangle_and_quad.cpp
    ex/common.h
    ex/common.cpp

    # Common source files:
    src/grid.h
    src/init_core.cpp
    src/init_core.h
    src/keyboard.h
    src/keyboard.cpp
    src/mouse.h
    src/mouse.cpp
    src/shader_prog.h
    src/shader_prog.cpp
    src/shape.h
    src/shape.cpp
)

# Compilation options:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        -O0 -g -ggdb
        -Wall -Wextra -Wno-unknown-pragmas
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        -Zi -Od -Oi
        -nologo -FC
        -EHa
        -W4
    )
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

# fmt library:
target_link_libraries(${PROJECT_NAME} PRIVATE fmt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt/include)

# OpenGL:
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
message(STATUS "[SANDBOX] Linking with OpenGL: ${OPENGL_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS})

# Glew:
find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_LIBRARIES})
message(STATUS "[SANDBOX] Linking with GLEW: ${GLEW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})

# GLFW:
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# Core library:
option(CORE_LIBRARY_SHARED "" ON)
target_link_libraries(${PROJECT_NAME} PRIVATE core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src ${CMAKE_CURRENT_BINARY_DIR}/core)

# Inlcude path:
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
