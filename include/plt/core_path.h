#pragma once

#include <core_API.h>
#include <core_cstr.h>
#include <core_traits.h>
#include <core_types.h>

namespace core {

static constexpr char PATH_SEPARATOR = '/';

template <typename T>
concept Path =
    requires {
        requires std::is_same_v<decltype(&T::data), const char* (T::*)() const>;
        requires std::is_same_v<decltype(&T::filePart), const char* (T::*)() const>;
        requires std::is_same_v<decltype(&T::extPart), const char* (T::*)() const>;
    };

struct CORE_API_EXPORT ImmutablePath {
    const char* ptr;
    const addr_size len;

    constexpr ImmutablePath(const char* p, addr_size l) : ptr(p), len(l) {}
    constexpr ImmutablePath(const char* p) : ptr(p), len(core::cstrLen(p)) {}

    constexpr const char* filePart() const {
        const char* p = ptr + len - 1;
        while (p != ptr && *p != PATH_SEPARATOR) {
            --p;
        }

        return p;
    }

    constexpr const char* extPart() const {
        const char* p = filePart();
        while (p != ptr && *p != '.') {
            --p;
        }

        return p;
    }

    constexpr const char* data() const { return ptr; }
};

static_assert(Path<ImmutablePath>, "ImmutablePath does not satisfy Path concept");

} // namespace core
