#pragma once

#include <core_cstr_conv.h>
#include <core_expected.h>
#include <core_str_view.h>
#include <core_types.h>

// FIXME:
// Write special formats for:
//   1. Integer number prefix with 0
//   2. Integer number round to n digits
//   3. Float number prefix with 0
//   4. Float number round to n digits after the dot
//   5. Replace all places where std::snprintf is used.

namespace core {

using namespace coretypes;

enum struct FormatError : u8 {
    TOO_FEW_ARGUMENTS,
    TOO_MANY_ARGUMENTS,
    OUT_BUFFER_OVERFLOW,
    FAILED_TO_CONVERT,
    SENTINEL
};

namespace detail {

template<typename T, typename... Args>
constexpr core::expected<u32, FormatError> formatImpl(char*& out, u32 remaining, const char* fmt, T value, Args... args);

} // namespace detail

constexpr core::expected<u32, FormatError> format(char* out, u32 outLen, const char* fmt) {
    u32 fmtLen = u32(core::cstrLen(fmt));
    if (outLen <= fmtLen) {
        return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
    }
    core::memcopy(out, fmt, fmtLen);
    return fmtLen;
}

template<typename... Args>
constexpr core::expected<u32, FormatError> format(char* out, u32 outLen, const char* fmt, Args... args) {
    char* ptr = out;
    char* orig = out;
    auto res = detail::formatImpl(ptr, outLen, fmt, args...);
    if (res.hasErr()) return core::unexpected(res.err());
    u32 written = u32(ptr - orig);
    return written;
}

template<typename... Args>
constexpr core::expected<u32, FormatError> format(core::Memory<char> out, const char* fmt, Args... args) {
    return format(out.data(), u32(out.len()), fmt, args...);
}

namespace detail {

// Default template resolution should fail:
template <typename T>
constexpr core::expected<u32, FormatError> convertToCStr(char*, u32, T) {
    // FIXME: I should write the address instead of asserting.
    static_assert(core::always_false<T>, "No conversion function for T available");
    return core::unexpected(FormatError::FAILED_TO_CONVERT);
}

template <typename T>
constexpr core::expected<u32, FormatError> convertInts(char* out, u32 outLen, T value) {
    auto res = core::intToCstr(value, out, outLen);
    if (res.hasErr()) {
        return core::unexpected(FormatError::FAILED_TO_CONVERT);
    }

    auto written = res.value();
    if (written >= outLen) {
        return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
    }

    return written;
}

template <typename T>
constexpr core::expected<u32, FormatError> convertFloats(char* out, u32 outLen, T value) {
    auto res = core::floatToCstr(value, out, outLen);
    if (res.hasErr()) {
        return core::unexpected(FormatError::FAILED_TO_CONVERT);
    }

    auto written = res.value();
    if (written >= outLen) {
        return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
    }

    return written;
}

constexpr core::expected<u32, FormatError> convertStrs(char* out, u32 outLen, const char* value, addr_size vlen) {
    if (outLen <= vlen) {
        return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
    }
    core::memcopy(out, value, vlen);
    return u32(vlen);
}

constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, i64 value) { return convertInts(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, i32 value) { return convertInts(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, i16 value) { return convertInts(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, i8 value)  { return convertInts(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, u64 value) { return convertInts(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, u32 value) { return convertInts(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, u16 value) { return convertInts(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, u8 value)  { return convertInts(out, outLen, value); }

constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, const char* value)   { return convertStrs(out, outLen, value, core::cstrLen(value)); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, char* value)         { return convertStrs(out, outLen, value, core::cstrLen(value)); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, core::StrView value) { return convertStrs(out, outLen, value.data(), value.len()); }

constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, bool value) {
    constexpr addr_size trueStrLen = core::cstrLen("true");
    constexpr addr_size falseStrLen = core::cstrLen("false");

    if (value) {
        return convertStrs(out, outLen, "true", trueStrLen);
    }

    return convertStrs(out, outLen, "false", falseStrLen);
}

constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, f32 value) { return convertFloats(out, outLen, value); }
constexpr core::expected<u32, FormatError> convertToCStr(char* out, u32 outLen, f64 value) { return convertFloats(out, outLen, value); }

template<typename T>
constexpr core::expected<u32, FormatError> appendArg(char*& out, u32& remaining, T value) {
    auto written = convertToCStr(out, remaining, value);
    if (written.hasErr()) return core::unexpected(written.err());
    out += written.value();
    remaining -= written.value();
    u32 ret = written.value();
    return ret;
}

// Base case: no more arguments. Simply copy the rest of the format string.
constexpr core::expected<u32, FormatError> formatImpl(char*& out, u32& remaining, const char* fmt) {
    u32 count = 0;
    while (*fmt) {
        if (fmt[0] == '{' && fmt[1] == '}') {
            return core::unexpected(FormatError::TOO_FEW_ARGUMENTS);
        }

        if (remaining <= 1) {
            return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
        }

        *out++ = *fmt++;
        --remaining;
        ++count;
    }

    if (remaining == 0) {
        return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
    }

    *out = '\0';
    return count;
}

template<typename T, typename... Args>
constexpr core::expected<u32, FormatError> formatImpl(char*& out, u32 remaining, const char* fmt, T value, Args... args) {
    u32 count = 0;
    while (*fmt) {
        if (fmt[0] == '{' && fmt[1] == '}') {
            fmt += 2; // Skip the "{}"

            auto argRes = appendArg(out, remaining, value);
            if (argRes.hasErr()) return core::unexpected(argRes.err());
            count += argRes.value();

            auto restRes = detail::formatImpl(out, remaining, fmt, args...);
            if (restRes.hasErr()) return core::unexpected(restRes.err());
            count += restRes.value();

            return count;
        }

        if (remaining <= 1) {
            return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
        }

        *out++ = *fmt++;
        --remaining;
        ++count;
    }

    return core::unexpected(FormatError::TOO_MANY_ARGUMENTS);
}

} // namespace detail

} // namespace core
