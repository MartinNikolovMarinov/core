#include "glfw_impl.h"

namespace app {

// KeyboardAction

KeyboardAction createKeyboardActionGLFW(i32 action) {
    using Type = KeyboardAction::Type;
    KeyboardAction ret;
    switch (action) {
        case GLFW_PRESS:   ret.type = Type::KEY_PRESS;   break;
        case GLFW_RELEASE: ret.type = Type::KEY_RELEASE; break;
        case GLFW_REPEAT:  ret.type = Type::KEY_REPEAT;  break;
        default:           ret.type = Type::NONE;        break;
    }
    return ret;
}

i32 getGLFWAction(KeyboardAction& a) {
    using Type = KeyboardAction::Type;
    switch (a.type) {
        case Type::KEY_PRESS:   return GLFW_PRESS;
        case Type::KEY_RELEASE: return GLFW_RELEASE;
        case Type::KEY_REPEAT:  return GLFW_REPEAT;
        default:                return -1;
    }
}

// KeyboardModifiers

KeyboardModifiers createKeyboardModifiersGLFW(i32 mods) {
    using Type = KeyboardModifiers::Type;
    KeyboardModifiers ret = { Type::NONE };
    if (mods & GLFW_MOD_SHIFT)     ret.type = (Type)(ret.type | Type::SHIFT);
    if (mods & GLFW_MOD_CONTROL)   ret.type = (Type)(ret.type | Type::CONTROL);
    if (mods & GLFW_MOD_ALT)       ret.type = (Type)(ret.type | Type::ALT);
    if (mods & GLFW_MOD_SUPER)     ret.type = (Type)(ret.type | Type::SUPER);
    if (mods & GLFW_MOD_CAPS_LOCK) ret.type = (Type)(ret.type | Type::CAPS_LOCK);
    if (mods & GLFW_MOD_NUM_LOCK)  ret.type = (Type)(ret.type | Type::NUM_LOCK);
    return ret;
}

i32 getGLFWMods(KeyboardModifiers& m) {
    using Type = KeyboardModifiers::Type;
    i32 ret = 0;
    if (m.type & Type::SHIFT)     ret |= GLFW_MOD_SHIFT;
    if (m.type & Type::CONTROL)   ret |= GLFW_MOD_CONTROL;
    if (m.type & Type::ALT)       ret |= GLFW_MOD_ALT;
    if (m.type & Type::SUPER)     ret |= GLFW_MOD_SUPER;
    if (m.type & Type::CAPS_LOCK) ret |= GLFW_MOD_CAPS_LOCK;
    if (m.type & Type::NUM_LOCK)  ret |= GLFW_MOD_NUM_LOCK;
    return ret;
}

// KeyInfo

namespace {

bool isGLFWMod(i32 value) {
    bool isModifier = value == GLFW_KEY_LEFT_SHIFT   || value == GLFW_KEY_RIGHT_SHIFT ||
                      value == GLFW_KEY_LEFT_CONTROL || value == GLFW_KEY_RIGHT_CONTROL ||
                      value == GLFW_KEY_LEFT_ALT     || value == GLFW_KEY_RIGHT_ALT ||
                      value == GLFW_KEY_LEFT_SUPER   || value == GLFW_KEY_RIGHT_SUPER ||
                      value == GLFW_KEY_CAPS_LOCK    || value == GLFW_KEY_NUM_LOCK;
    return isModifier;
}

} // namespace

KeyInfo createKeyInfoGLFW(i32 key, i32 scancode, i32 action) {
    KeyInfo info;
    info.value = key;
    info.scancode = scancode;
    info.isMod = isGLFWMod(key);
    info.action = createKeyboardActionGLFW(action);
    return info;
}

// Keyboard

template<>
void GLFWKeyboard::setKey(KeyInfo&& key) {
    keys[key.scancode] = core::move(key);
}

template<>
KeyInfo& GLFWKeyboard::getKey(i32 scancode) {
    return keys[scancode];
}

template<>
void GLFWKeyboard::setModifiers(KeyboardModifiers&& mods) {
    modifiers = core::move(mods);
}

template<>
KeyboardModifiers& GLFWKeyboard::getModifiers() {
    return modifiers;
}

template<>
void GLFWKeyboard::setTextInput(core::rune&& text) {
    textInput = core::move(text);
}

template<>
core::rune& GLFWKeyboard::getTextInput() {
    return textInput;
}

template<>
void GLFWKeyboard::update() {
    // Keys that were released last frame are unset this frame.
    for (i32 i = 0; i < GLFW_KEY_LAST; i++) {
        if (keys[i].isReleased()) {
            keys[i].action.type = KeyboardAction::Type::NONE;
        }
    }
    textInput = core::rune{0};
}

template<>
std::string GLFWKeyboard::toString() const {
    std::string result = "Keyboard: { ";
    result += "keys: [ ";
    for (i32 i = 0; i < GLFW_KEY_LAST; i++) {
        if (keys[i].action.type != KeyboardAction::Type::NONE) {
            result += keys[i].toString();
            if (i != GLFW_KEY_LAST - 1) result += ", ";
        }
    }
    result += " ], ";
    result += "modifiers: " + std::string(modifiers.toCptr()) + ", ";
    char utfChar[4];
    core::rune_to_bytes(textInput, (uchar*)utfChar);
    result += "textInput: " + std::string(utfChar);
    result += " }";
    result += '\n';
    return result;
}

} // namespace app
