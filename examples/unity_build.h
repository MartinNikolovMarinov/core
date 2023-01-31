#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fmt/format.h>

// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include "init_core.h"
#include "init_core.cpp"

#include "shader_prog.h"
#include "shader_prog.cpp"
#include "mesh.h"
#include "mesh.cpp"
#include "keyboard.h"
#include "keyboard.cpp"
#include "mouse.h"
#include "mouse.cpp"
