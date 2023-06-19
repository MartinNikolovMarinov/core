#pragma once

#include "init_core.h"

#include <string>

namespace app {

using namespace coretypes;

struct KeyboardAction {
    enum Type : u8 {
        NONE = 0,
        KEY_PRESS = 1,
        KEY_RELEASE = 2,
        KEY_REPEAT = 3,

        SENTINEL
    };

    Type type = Type::NONE;

    bool operator==(const KeyboardAction& other) { return type == other.type; }

    const char* toCptr() const;
};

struct KeyboardModifiers {
    enum Type : i8 {
        NONE = 0,
        SHIFT = 1 << 0,
        CONTROL = 1 << 1,
        ALT = 1 << 2,
        SUPER = 1 << 3,
        CAPS_LOCK = 1 << 4,
        NUM_LOCK = 1 << 5,

        SENTINEL
    };

    Type type = Type::NONE;

    void combine(const KeyboardModifiers& other);
    void remove(const KeyboardModifiers& other);

    const char* toCptr() const;
};

struct KeyInfo {
    i32 value = 0;
    i32 scancode = 0;
    bool isMod = false;
    KeyboardAction action = { KeyboardAction::Type::NONE };

    bool operator==(const KeyInfo& other) const;

    bool isPressed() const;
    bool isReleased() const;
    bool isRepeatd() const;
    bool isModifier() const;

    std::string toString() const;
};

// NOTE: Keyboard needs to be implemtend for the specific rendering API. No generic implementation is provided.
template<i32 TKeyCount>
struct IKeyboard {
    KeyInfo keys[TKeyCount] = {};
    KeyboardModifiers modifiers = { KeyboardModifiers::Type::NONE };
    core::rune textInput;

    KeyInfo&           getKey(i32 scancode);
    void               setKey(KeyInfo&& key);
    KeyboardModifiers& getModifiers();
    void               setModifiers(KeyboardModifiers&& mods);
    core::rune&        getTextInput();
    void               setTextInput(core::rune&& text);

    void update();

    std::string toString() const;
};

} // namespace app
