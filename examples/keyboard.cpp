#include "keyboard.h"

bool keyboard_action::operator==(const keyboard_action& other) const {
    return type == other.type;
}

keyboard_action keyboard_action::create_from_glfw(i32 action) {
    keyboard_action ret;
    switch (action) {
        case GLFW_PRESS:   ret.type = Type::KEY_PRESS;   break;
        case GLFW_RELEASE: ret.type = Type::KEY_RELEASE; break;
        case GLFW_REPEAT:  ret.type = Type::KEY_REPEAT;  break;
        default:           ret.type = Type::NONE;       break;
    }
    return ret;
}

i32 keyboard_action::to_glfw_action() {
    switch (type) {
        case Type::KEY_PRESS:   return GLFW_PRESS;
        case Type::KEY_RELEASE: return GLFW_RELEASE;
        case Type::KEY_REPEAT:  return GLFW_REPEAT;
        default:               return -1;
    }
}

const char* keyboard_action::to_cptr() {
    switch (type) {
        case Type::KEY_PRESS:   return "key press";
        case Type::KEY_RELEASE: return "key release";
        case Type::KEY_REPEAT:  return "key repeat";
        default:               return "not set";
    }
}

keyboard_modifiers keyboard_modifiers::create_from_glfw(i32 mods) {
    keyboard_modifiers ret = { Type::NONE };
    if (mods & GLFW_MOD_SHIFT)     ret.type = (Type)(ret.type | Type::SHIFT);
    if (mods & GLFW_MOD_CONTROL)   ret.type = (Type)(ret.type | Type::CONTROL);
    if (mods & GLFW_MOD_ALT)       ret.type = (Type)(ret.type | Type::ALT);
    if (mods & GLFW_MOD_SUPER)     ret.type = (Type)(ret.type | Type::SUPER);
    if (mods & GLFW_MOD_CAPS_LOCK) ret.type = (Type)(ret.type | Type::CAPS_LOCK);
    if (mods & GLFW_MOD_NUM_LOCK)  ret.type = (Type)(ret.type | Type::NUM_LOCK);
    return ret;
}

i32 keyboard_modifiers::to_glfw_mods() {
    i32 ret = 0;
    if (type & Type::SHIFT)     ret |= GLFW_MOD_SHIFT;
    if (type & Type::CONTROL)   ret |= GLFW_MOD_CONTROL;
    if (type & Type::ALT)       ret |= GLFW_MOD_ALT;
    if (type & Type::SUPER)     ret |= GLFW_MOD_SUPER;
    if (type & Type::CAPS_LOCK)  ret |= GLFW_MOD_CAPS_LOCK;
    if (type & Type::NUM_LOCK)   ret |= GLFW_MOD_NUM_LOCK;
    return ret;
}

const char* keyboard_modifiers::to_cptr() {
    if (type == Type::NONE)
        return "none";
    if (type == Type::SHIFT)
        return "shift";
    if (type == Type::CONTROL)
        return "control";
    if (type == Type::ALT)
        return "alt";
    if (type == Type::SUPER)
        return "super";
    if (type == Type::CAPS_LOCK)
        return "caps lock";
    if (type == Type::NUM_LOCK)
        return "num lock";
    if (type == (Type::SHIFT | Type::CONTROL))
        return "shift + control";
    if (type == (Type::SHIFT | Type::ALT))
        return "shift + alt";
    if (type == (Type::SHIFT | Type::SUPER))
        return "shift + super";
    if (type == (Type::SHIFT | Type::CAPS_LOCK))
        return "shift + caps lock";
    if (type == (Type::SHIFT | Type::NUM_LOCK))
        return "shift + num lock";
    if (type == (Type::CONTROL | Type::ALT))
        return "control + alt";
    if (type == (Type::CONTROL | Type::SUPER))
        return "control + super";
    if (type == (Type::CONTROL | Type::CAPS_LOCK))
        return "control + caps lock";
    if (type == (Type::CONTROL | Type::NUM_LOCK))
        return "control + num lock";
    if (type == (Type::ALT | Type::SUPER))
        return "alt + super";
    if (type == (Type::ALT | Type::CAPS_LOCK))
        return "alt + caps lock";
    if (type == (Type::ALT | Type::NUM_LOCK))
        return "alt + num lock";
    if (type == (Type::SUPER | Type::CAPS_LOCK))
        return "super + caps lock";
    if (type == (Type::SUPER | Type::NUM_LOCK))
        return "super + num lock";
    if (type == (Type::CAPS_LOCK | Type::NUM_LOCK))
        return "caps lock + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT))
        return "shift + control + alt";
    if (type == (Type::SHIFT | Type::CONTROL | Type::SUPER))
        return "shift + control + super";
    if (type == (Type::SHIFT | Type::CONTROL | Type::CAPS_LOCK))
        return "shift + control + caps lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::NUM_LOCK))
        return "shift + control + num lock";
    if (type == (Type::SHIFT | Type::ALT | Type::SUPER))
        return "shift + alt + super";
    if (type == (Type::SHIFT | Type::ALT | Type::CAPS_LOCK))
        return "shift + alt + caps lock";
    if (type == (Type::SHIFT | Type::ALT | Type::NUM_LOCK))
        return "shift + alt + num lock";
    if (type == (Type::SHIFT | Type::SUPER | Type::CAPS_LOCK))
        return "shift + super + caps lock";
    if (type == (Type::SHIFT | Type::SUPER | Type::NUM_LOCK))
        return "shift + super + num lock";
    if (type == (Type::SHIFT | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "shift + caps lock + num lock";
    if (type == (Type::CONTROL | Type::ALT | Type::SUPER))
        return "control + alt + super";
    if (type == (Type::CONTROL | Type::ALT | Type::CAPS_LOCK))
        return "control + alt + caps lock";
    if (type == (Type::CONTROL | Type::ALT | Type::NUM_LOCK))
        return "control + alt + num lock";
    if (type == (Type::CONTROL | Type::SUPER | Type::CAPS_LOCK))
        return "control + super + caps lock";
    if (type == (Type::CONTROL | Type::SUPER | Type::NUM_LOCK))
        return "control + super + num lock";
    if (type == (Type::CONTROL | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "control + caps lock + num lock";
    if (type == (Type::ALT | Type::SUPER | Type::CAPS_LOCK))
        return "alt + super + caps lock";
    if (type == (Type::ALT | Type::SUPER | Type::NUM_LOCK))
        return "alt + super + num lock";
    if (type == (Type::ALT | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "alt + caps lock + num lock";
    if (type == (Type::SUPER | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "super + caps lock + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT | Type::SUPER))
        return "shift + control + alt + super";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT | Type::CAPS_LOCK))
        return "shift + control + alt + caps lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT | Type::NUM_LOCK))
        return "shift + control + alt + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::SUPER | Type::CAPS_LOCK))
        return "shift + control + super + caps lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::SUPER | Type::NUM_LOCK))
        return "shift + control + super + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "shift + control + caps lock + num lock";
    if (type == (Type::SHIFT | Type::ALT | Type::SUPER | Type::CAPS_LOCK))
        return "shift + alt + super + caps lock";
    if (type == (Type::SHIFT | Type::ALT | Type::SUPER | Type::NUM_LOCK))
        return "shift + alt + super + num lock";
    if (type == (Type::SHIFT | Type::ALT | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "shift + alt + caps lock + num lock";
    if (type == (Type::CONTROL | Type::ALT | Type::SUPER | Type::NUM_LOCK))
        return "control + alt + super + num lock";
    if (type == (Type::CONTROL | Type::ALT | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "control + alt + caps lock + num lock";
    if (type == (Type::CONTROL | Type::SUPER | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "control + super + caps lock + num lock";
    if (type == (Type::ALT | Type::SUPER | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "alt + super + caps lock + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT | Type::SUPER | Type::CAPS_LOCK))
        return "shift + control + alt + super + caps lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT | Type::SUPER | Type::NUM_LOCK))
        return "shift + control + alt + super + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "shift + control + alt + caps lock + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::SUPER | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "shift + control + super + caps lock + num lock";
    if (type == (Type::SHIFT | Type::ALT | Type::SUPER | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "shift + alt + super + caps lock + num lock";
    if (type == (Type::CONTROL | Type::ALT | Type::SUPER | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "control + alt + super + caps lock + num lock";
    if (type == (Type::SHIFT | Type::CONTROL | Type::ALT | Type::SUPER | Type::CAPS_LOCK | Type::NUM_LOCK))
        return "shift + control + alt + super + caps lock + num lock";
    return "unknown";
}

bool key_info::operator==(const key_info& other) const {
    return scancode == other.scancode;
}

key_info key_info::create_from_glfw(i32 key, i32 scancode, i32 action) {
    key_info info;
    info.value = key;
    info.scancode = scancode;
    info.action = keyboard_action::create_from_glfw(action);
    return info;
}

std::string key_info::to_string() {
    std::string result = "key_info: { ";
    result += "value: " + std::to_string(value) + ", ";
    result += "scancode: " + std::to_string(scancode) + ", ";
    result += "action: " + std::string(action.to_cptr());
    result += " }";
    return result;
}

keyboard& keyboard::set_key(key_info&& key) {
    keys[key.scancode] = core::move(key);
    return *this;
}

key_info& keyboard::get_key(i32 scancode) {
    return keys[scancode];
}

keyboard& keyboard::set_modifiers(keyboard_modifiers&& mods) {
    modifiers = core::move(mods);
    return *this;
}

keyboard_modifiers& keyboard::get_modifiers() {
    return modifiers;
}

keyboard& keyboard::set_text_input(core::rune&& text) {
    textInput = core::move(text);
    return *this;
}

core::rune& keyboard::get_text_input() {
    return textInput;
}

std::string keyboard::to_string() {
    std::string result = "Keyboard: { ";
    result += "keys: [ ";
    for (i32 i = 0; i < GLFW_KEY_LAST; i++) {
        if (keys[i].action.type == keyboard_action::Type::KEY_PRESS ||
            keys[i].action.type == keyboard_action::Type::KEY_REPEAT
        ) {
            result += keys[i].to_string();
            if (i != GLFW_KEY_LAST - 1) result += ", ";
        }
    }
    result += " ], ";
    result += "modifiers: " + std::string(modifiers.to_cptr()) + ", ";
    uchar uftChar[4];
    core::rune_to_bytes(textInput, uftChar);
    result += "textInput: " + std::string((char*)uftChar);
    result += " }";
    return result;
}

