#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_cptr.h>
#include <core_mem.h>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT StrView {
    using size_type = addr_size;

    constexpr StrView() : ptr(nullptr), length(0) {}
    constexpr StrView(const char* _ptr, size_type _len) : ptr(_ptr), length(_len) {}
    constexpr StrView(const char* _ptr) : ptr(_ptr), length(core::cptrLen(_ptr)) {}
    constexpr StrView(const StrView&) = default;
    constexpr StrView(StrView&&) = default;

    constexpr StrView& operator=(const char*) = delete;
    constexpr StrView& operator=(const StrView&) = default;
    constexpr StrView& operator=(StrView&&) = default;

    constexpr const char* data() const { return ptr; }
    constexpr size_type   len()  const { return length; }

    constexpr operator bool() const { return ptr != nullptr; }

    constexpr const char& operator[](size_type idx) const { return ptr[idx]; }

    constexpr bool eq(const StrView& other) const {
        bool areEqual = core::cptrCmp(ptr, length, other.ptr, other.length) == 0;
        return areEqual;
    }

    const char* ptr;
    size_type length;
};

constexpr StrView sv()                               { return StrView{nullptr, 0}; }
constexpr StrView sv(const char* str)                { return StrView{str, core::cptrLen(str)}; }
constexpr StrView sv(const char* str, addr_size len) { return StrView{str, len}; }

constexpr StrView operator""_sv(const char* str, size_t len) {
    return StrView(str, static_cast<StrView::size_type>(len));
}

} // namespace core

using core::operator""_sv; // use string literal globally.
