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
    src/box.h
    src/box.cpp

    # Common glfw source files:
    src/glfw/glfw_impl.h
    src/glfw/glfw_keyboard.cpp
    src/glfw/glfw_mouse.cpp

    # Examples:
    ex/raytracing.h
    ex/raytracing.cpp
    ex/ray_in_voxel_space.h
    ex/ray_in_voxel_space.cpp
    ex/keyboard_and_mouse.h
    ex/keyboard_and_mouse.cpp
    ex/experiment_01_triangle_and_quad.h
    ex/experiment_01_triangle_and_quad.cpp
    ex/experiment_02_textures.h
    ex/experiment_02_textures.cpp
    ex/experiment_03_transformations.h
    ex/experiment_03_transformations.cpp
    ex/experiment_04_rotating_cube.h
    ex/experiment_04_rotating_cube.cpp
    ex/experiment_05_camera.h
    ex/experiment_05_camera.cpp
    ex/common.h
    ex/common.cpp
)

# BEGIN Compile Options

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(mm_flags -std=c++17 -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function)
    set(mm_core_debug_flags -O0 -g -ggdb)
    set(mm_core_release_flags)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(mm_flags
        /std:c++17
        /nologo /FC
        /W4
        /EHsc
    )
    set(mm_core_debug_flags /O0 /Zi /Od /Oi)
    set(mm_core_release_flags /O2)
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE ${mm_flags} ${mm_core_debug_flags})
else()
    target_compile_options(${PROJECT_NAME} PRIVATE ${mm_flags} ${mm_core_release_flags})
endif()

# END Compile Options

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
option(CORE_DEBUG "" ON)
option(CORE_ASSERT_ENABLED "" ON)
option(CORE_LIBRARY_SHARED "" ON)
option(CORE_BUILD_TESTS "" OFF)
option(RUN_COMPILETIME_TESTS "" OFF)
target_link_libraries(${PROJECT_NAME} PRIVATE core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/core)

# STB single header libraries:
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb)

# GLM:
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm)

# Inlcude path:
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
