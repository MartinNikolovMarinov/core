#include "../keyboard.h"

#include <GLFW/glfw3.h>

namespace app {

KeyboardAction    createKeyboardActionGLFW(i32 action);
KeyboardModifiers createKeyboardModifiersGLFW(i32 mods);
KeyInfo           createKeyInfoGLFW(i32 key, i32 scancode, i32 action);

i32 getGLFWAction(KeyboardAction& a);
i32 getGLFWMods(KeyboardModifiers& m);
i32 getGLFWKey(KeyInfo& k);

using GLFWKeyboard = IKeyboard<GLFW_KEY_LAST>;

};
