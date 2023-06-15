#include "keyboard.h"

namespace app {

const char* KeyboardAction::toCptr() const {
    switch (type) {
        case Type::KEY_PRESS:   return "key press";
        case Type::KEY_RELEASE: return "key release";
        case Type::KEY_REPEAT:  return "key repeat";
        default:                return "not set";
    }
}

void KeyboardModifiers::combine(const KeyboardModifiers& other) {
    type = (Type)(type | other.type);
}

void KeyboardModifiers::remove(const KeyboardModifiers& other) {
    type = (Type)(type & ~other.type);
}

const char* KeyboardModifiers::toCptr() const {
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

bool KeyInfo::operator==(const KeyInfo& other) const { return scancode == other.scancode; }

bool KeyInfo::isPressed()  const { return action.type == KeyboardAction::Type::KEY_PRESS; }
bool KeyInfo::isReleased() const { return action.type == KeyboardAction::Type::KEY_RELEASE; }
bool KeyInfo::isRepeatd()  const { return action.type == KeyboardAction::Type::KEY_REPEAT; }
bool KeyInfo::isModifier() const { return isMod; }

std::string KeyInfo::toString() const {
    std::string result = "key_info: { ";
    result += "value: " + std::to_string(value) + ", ";
    result += "scancode: " + std::to_string(scancode) + ", ";
    result += "is_modifier: " + std::to_string(isMod) + ", ";
    result += "action: " + std::string(action.toCptr());
    result += " }";
    return result;
}

} // namespace app
