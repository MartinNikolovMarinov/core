#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_cstr.h>
#include <core_mem.h>

namespace core {

using namespace coretypes;

using StrView = core::Memory<const char>;

constexpr StrView sv()                               { return StrView{nullptr, 0}; }
constexpr StrView sv(const char* str)                { return StrView{str, core::cstrLen(str)}; }
constexpr StrView sv(const char* str, addr_size len) { return StrView{str, len}; }

constexpr StrView operator""_sv(const char* str, size_t len) {
    return StrView(str, static_cast<StrView::size_type>(len));
}

} // namespace core

using core::operator""_sv; // use string literal globally.
