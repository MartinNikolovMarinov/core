cmake_minimum_required(VERSION 3.2)

project(main)

# Global CMake options:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Create executable:
add_executable(${PROJECT_NAME}
    main.cpp

    # Common source files:
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

    # Examples:
    ex/experiment_app.h
    ex/experiment_app.cpp
    ex/raytracing.h
    ex/raytracing.cpp
    ex/ray_in_voxel_space.h
    ex/ray_in_voxel_space.cpp
    ex/experiment_01_triangle_and_quad.h
    ex/experiment_01_triangle_and_quad.cpp
    ex/experiment_02_textures.h
    ex/experiment_02_textures.cpp
    ex/experiment_03_transformations.h
    ex/experiment_03_transformations.cpp
    ex/experiment_04_rotating_cube.h
    ex/experiment_04_rotating_cube.cpp
    ex/common.h
    ex/common.cpp
)

# Compilation options:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Common flags:
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wno-unknown-pragmas
    )

    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # Release flags:
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -O3)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Debug flags:
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -O0 -g -ggdb)
    else()
        message(FATAL_ERROR "Unsupported build type!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Common flags:
    target_compile_options(${PROJECT_NAME} PRIVATE
        /std:c++17 /nologo /FC /EHa /W4
    )

    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
        # Debug flags:
        target_compile_options(${PROJECT_NAME} PRIVATE /O0 /Zi /Od /Oi)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Release flags:
        target_compile_options(${PROJECT_NAME} PRIVATE /O2)
    else()
        message(FATAL_ERROR "Unsupported build type!")
    endif()
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

# Definitions:
target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DPATH_TO_DATA="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

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
option(CORE_ASSERT_ENABLED "" ON)
target_link_libraries(${PROJECT_NAME} PRIVATE core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src ${CMAKE_CURRENT_BINARY_DIR}/core)

# STB single header libraries:
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb)

# GLM:
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm)

# Inlcude path:
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
