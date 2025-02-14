#pragma once

#include <core_cstr_conv.h>
#include <core_expected.h>
#include <core_str_view.h>
#include <core_types.h>

// FIXME:
// Write special formats for:
//   1. Integer number prefix with 0                    {03:}
//   2. Integer number round to n digits                {:d5}
//   3. Float number prefix with 0                      {03:}
//   4. Float number round to n digits after the dot    {04:f4.3}
//   5. Print nullptrs and try to print addresses of unknown types.
//   6. Print things as hexadecimal representation.     {03:h}, {03:H}
//   7. Print things as binary representation.          {03:b}, {03:B}
//   8. Replace all places where std::snprintf is used.

namespace core {

using namespace coretypes;

enum struct FormatError : u8 {
    INVALID_ARGUMENTS,
    TOO_FEW_ARGUMENTS,
    TOO_MANY_ARGUMENTS,
    OUT_BUFFER_OVERFLOW,
    INVALID_PLACEHOLDER,
    SENTINEL
};

namespace detail {

template<typename T, typename... Args>
constexpr core::expected<i32, FormatError> formatImpl(char*& out, i32 remaining, const char* fmt, T value, Args... args);

} // namespace detail

constexpr core::expected<i32, FormatError> format(char* out, i32 outLen, const char* fmt) {
    if (out == nullptr) return core::unexpected(FormatError::INVALID_ARGUMENTS);
    if (fmt == nullptr) return core::unexpected(FormatError::INVALID_ARGUMENTS);
    i32 fmtLen = i32(core::cstrLen(fmt));
    if (outLen <= fmtLen) {
        return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
    }
    core::memcopy(out, fmt, addr_size(fmtLen));
    return fmtLen;
}

template<typename... Args>
constexpr core::expected<i32, FormatError> format(char* out, i32 outLen, const char* fmt, Args... args) {
    if (out == nullptr) return core::unexpected(FormatError::INVALID_ARGUMENTS);
    if (fmt == nullptr) return core::unexpected(FormatError::INVALID_ARGUMENTS);
    char* ptr = out;
    char* orig = out;
    auto res = detail::formatImpl(ptr, outLen, fmt, args...);
    if (res.hasErr()) return core::unexpected(res.err());
    i32 written = i32(ptr - orig);
    return written;
}

template<typename... Args>
constexpr core::expected<i32, FormatError> format(core::Memory<char> out, const char* fmt, Args... args) {
    return format(out.data(), i32(out.len()), fmt, args...);
}

namespace detail {

struct PlaceHolderOptions {
    struct Hex { bool upperCase; };
    struct FixedFloat { u32 fixed; };

    enum struct Type {
        Invalid,
        Empty,
        PaddingOnly,
        Hex,
        Binary,
        FixedFloat
    };

    Type type;
    u16 padding;
    char paddingSymbol;
    union {
        Hex hex;
        FixedFloat fixedFloat;
    };

    static constexpr PlaceHolderOptions parse(i32& remaining, const char*& fmt) {
        PlaceHolderOptions options;
        options.type = Type::Invalid;
        options.padding = 0;
        options.paddingSymbol = '\0';

        if (*fmt == '}') [[likely]] {
            // This will be the most likely path for most placeholders.
            if (remaining <= 1) return options;
            remaining--;
            fmt++;
            options.type = Type::Empty;
            return options;
        }

        // More complicated state machine processing is necessary.

        /**
         * State meanings:
         *   0 - in padding
         *   1 - in options
         *   2 - in hex
         *   3 - in binary
         *   4 - in fixed float
         *   5 - found closing bracket
        */
        u8 state = 0;
        constexpr u32 BUFF_SIZE = 5;
        char buff[BUFF_SIZE + 1] = {};
        u8 buffWriteIdx = 0;
        Type deducedType = Type::Invalid;

        // Example formatting:
        // {}, {03:}, {03:h}, {03:H}, {03:b}, {03:B}, {04:f.3}

        for (; *fmt; fmt++) {
            // Before doing anything update the remaining bytes in the output buffer.
            if (remaining <= 1) break;
            remaining--;

            if (*fmt == ':') {
                if (buffWriteIdx > 0) {
                    // The first character is the padding symbol
                    options.paddingSymbol = *buff;

                    if (buffWriteIdx <= 1) break; // No repeat count for paddding symbol

                    // The sequence after that is the padding repeat count:
                    auto res = core::cstrToInt<u16>(buff + 1, u32(core::cstrLen(buff + 1)));
                    if (res.hasErr()) break;
                    options.padding = res.value();

                    // Clean the buffer for the next state.
                    buffWriteIdx = 0;
                    core::memset(buff, '\0', BUFF_SIZE);
                }

                if (*(fmt + 1) == '}') {
                    deducedType = Type::PaddingOnly;
                }

                state = 1;
                continue;
            }

            if (*fmt == '}') {
                // Successfully reached a closing bracket.
                options.type = deducedType;
                state = 5;
                fmt++;
                break;
            }

            if (state == 0) {
                if (buffWriteIdx + 1 >= BUFF_SIZE) {
                    // Overflow, padding above a certail digit count is just not useful.
                    break;
                }
                if (buffWriteIdx > 1 && !core::isDigit(*fmt)) {
                    // Only the first argument in the padding options is allowed to be non-digit.
                    break;
                }
                buff[buffWriteIdx++] = *fmt;
            }
            else if (state == 1) {
                if (*fmt == 'h' || *fmt == 'H') {
                    deducedType = Type::Hex;
                    options.hex.upperCase = (*fmt == 'H');
                    state = 2;
                    continue;
                }
                else if (*fmt == 'b') {
                    deducedType = Type::Binary;
                    state = 3;
                    continue;
                }
                else if (*fmt == 'f') {
                    deducedType = Type::FixedFloat;
                    state = 4;
                    continue;
                }

                // Non of the expected formatting options.
                break;
            }
            else if (state == 2 || state == 3) {
                // Hex and binary format don't expect anything more than a SINGLE letter, anything else is an error.
                break;
            }
            else if (state == 4) {
                if (*fmt == '.') continue;
                if (!core::isDigit(*fmt)) break;
                if (buffWriteIdx + 1 >= BUFF_SIZE) {
                    // Overflow, padding above a certail digit count is just not useful.
                    break;
                }
                buff[buffWriteIdx++] = *fmt;
            }
        }

        return options;
    }
};

// Default template resolution should fail:
template <typename T>
constexpr core::expected<i32, FormatError> convertToCStr(char*, i32, T, PlaceHolderOptions&) {
    static_assert(core::always_false<T>, "T is not a basic type that supports formatting");
    return core::unexpected(FormatError::SENTINEL);
}

template <typename T>
constexpr core::expected<i32, FormatError> convertInts(char* out, i32 outLen, T value, [[maybe_unused]] PlaceHolderOptions& options) {
    switch (options.type) {
        case PlaceHolderOptions::Type::Empty: {
            auto res = core::intToCstr(value, out, u32(outLen));
            if (res.hasErr()) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }

            auto written = i32(res.value());
            if (written >= outLen) [[unlikely]] {
                // This probably means that intToCstr has a bug.
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }
        }

        case PlaceHolderOptions::Type::PaddingOnly: {
            i32 dc = i32(core::digitCount(value));
            i32 computedLen = core::core_max(i32(options.padding), dc);
            if (outLen <= computedLen) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }

            i32 repeatCount = options.padding - dc;
            if (repeatCount > 0) {
                out += core::memset(out, options.paddingSymbol, addr_size(repeatCount));
            }

            core::Unpack(core::intToCstr(value, out, u32(outLen), u32(dc)));
            return computedLen;
        }

        case PlaceHolderOptions::Type::Hex: {
            Panic(false, "TODO:");
        }

        case PlaceHolderOptions::Type::Binary: {
            Panic(false, "TODO:");
        }

        case PlaceHolderOptions::Type::Invalid:     [[fallthrough]];
        case PlaceHolderOptions::Type::FixedFloat:  [[fallthrough]];
        default:                                    break;
    }

    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
}

template <typename T>
constexpr core::expected<i32, FormatError> convertFloats(char* out, i32 outLen, T value, [[maybe_unused]] PlaceHolderOptions& options) {
    switch (options.type) {
        case PlaceHolderOptions::Type::Empty: {
            auto res = core::floatToCstr(value, out, u32(outLen));
            if (res.hasErr()) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }

            auto written = i32(res.value());
            if (written >= outLen) [[unlikely]] {
                // This probably means that floatToCstr has a bug.
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }
        }

        case PlaceHolderOptions::Type::PaddingOnly: {
            Panic(false, "TODO:");
        }

        case PlaceHolderOptions::Type::FixedFloat: {
            Panic(false, "TODO:");
        }

        case PlaceHolderOptions::Type::Hex:     [[fallthrough]];
        case PlaceHolderOptions::Type::Binary:  [[fallthrough]];
        case PlaceHolderOptions::Type::Invalid: [[fallthrough]];
        default:                                break;
    }

    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
}

constexpr core::expected<i32, FormatError> convertStrs(char* out, i32 outLen, const char* value, i32 vlen, PlaceHolderOptions& options) {
    if (vlen <= 0) return 0;

    switch (options.type) {
        case PlaceHolderOptions::Type::Empty: {
            if (outLen <= vlen) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }

            core::memcopy(out, value, addr_size(vlen));
            return vlen;
        }

        case PlaceHolderOptions::Type::PaddingOnly: {
            i32 computedLen = core::core_max(i32(options.padding), vlen);
            if (outLen <= computedLen) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }

            i32 repeatCount = options.padding - vlen;
            if (repeatCount > 0) {
                out += core::memset(out, options.paddingSymbol, addr_size(repeatCount));
            }

            core::memcopy(out, value, addr_size(vlen));
            return computedLen;
        }

        case PlaceHolderOptions::Type::Hex: {
            i32 spacesBetweenElements = vlen * i32(sizeof(' ')) - 1; // -1 because the last element does not put a space.
            i32 spaceForOneHexElement = i32(sizeof(u8) << 1);
            i32 totalSize = spaceForOneHexElement * vlen + spacesBetweenElements;

            if (options.padding > 0) break; // Padding is not allowed in char array to hex.
            if (outLen <= totalSize) break; // Overflow

            for (i32 i = 0; i < vlen; i++) {
                char curr = value[i];
                out += core::Unpack(core::intToHex(curr, out, addr_size(spaceForOneHexElement + 1), options.hex.upperCase));
                if (i != vlen - 1) *out++ = ' ';
            }

            return totalSize;
        }

        case PlaceHolderOptions::Type::Binary: {
            i32 spacesBetweenElements = vlen * i32(sizeof(' ')) - 1; // -1 because the last element does not put a space.
            i32 spaceForOneBinaryElement = core::BYTE_SIZE;
            i32 totalSize = spaceForOneBinaryElement * vlen + spacesBetweenElements;

            if (options.padding > 0) break; // Padding is not allowed in char array to binary.
            if (outLen <= totalSize) break; // Overflow

            for (i32 i = 0; i < vlen; i++) {
                char curr = value[i];
                out += core::Unpack(core::intToBinary(curr, out, addr_size(spaceForOneBinaryElement + 1)));
                if (i != vlen - 1) *out++ = ' ';
            }

            return totalSize;
        }

        case PlaceHolderOptions::Type::Invalid:     [[fallthrough]];
        case PlaceHolderOptions::Type::FixedFloat:  [[fallthrough]];
        default:                                    break;
    }

    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
}

constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, i64 value, PlaceHolderOptions& options) { return convertInts(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, i32 value, PlaceHolderOptions& options) { return convertInts(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, i16 value, PlaceHolderOptions& options) { return convertInts(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, i8 value, PlaceHolderOptions& options)  { return convertInts(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, u64 value, PlaceHolderOptions& options) { return convertInts(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, u32 value, PlaceHolderOptions& options) { return convertInts(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, u16 value, PlaceHolderOptions& options) { return convertInts(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, u8 value, PlaceHolderOptions& options)  { return convertInts(out, outLen, value, options); }

constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, const char* value, PlaceHolderOptions& options)   { return convertStrs(out, outLen, value, i32(core::cstrLen(value)), options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, core::StrView value, PlaceHolderOptions& options) { return convertStrs(out, outLen, value.data(), i32(value.len()), options); }

constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, bool value, PlaceHolderOptions& options) {
    if (options.padding > 0) {
        // boolean type does not support padding
        return core::unexpected(FormatError::INVALID_ARGUMENTS);
    }
    if (options.type != PlaceHolderOptions::Type::Empty) {
        // boolean type supports only empty placeholders
        return core::unexpected(FormatError::INVALID_ARGUMENTS);
    }

    constexpr addr_size trueStrLen = core::cstrLen("true");
    constexpr addr_size falseStrLen = core::cstrLen("false");

    if (value) {
        return convertStrs(out, outLen, "true", trueStrLen, options);
    }

    return convertStrs(out, outLen, "false", falseStrLen, options);
}

constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, f32 value, PlaceHolderOptions& options) { return convertFloats(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, f64 value, PlaceHolderOptions& options) { return convertFloats(out, outLen, value, options); }

template<typename T>
constexpr core::expected<i32, FormatError> appendArg(char*& out, i32& remaining, T value, PlaceHolderOptions& options) {
    auto written = convertToCStr(out, remaining, value, options);
    if (written.hasErr()) return core::unexpected(written.err());
    out += written.value();
    remaining -= written.value();
    i32 ret = written.value();
    return ret;
}

constexpr inline bool escapeBrackets(char*& out, i32& remaining, const char*& fmt, i32& count) {
    if (
        (fmt[0] == '{' && fmt[1] == '{') ||
        (fmt[0] == '}' && fmt[1] == '}')
    ) {
        *out++ = *fmt; // write only one bracket.
        remaining--;
        fmt += 2;
        count += 2;
        return true;
    }

    return false;
}

// Base case: no more arguments. Simply copy the rest of the format string.
constexpr core::expected<i32, FormatError> formatImpl(char*& out, i32& remaining, const char* fmt) {
    i32 count = 0;
    while (*fmt) {
        if (escapeBrackets(out, remaining, fmt, count)) {
            continue;
        }

        if (fmt[0] == '{' || fmt[0] == '}') {
            // Unescaped brackets are considered placeholders, so if there are any placeholders in the base case that
            // means the provided arguments where too few.
            return core::unexpected(FormatError::TOO_FEW_ARGUMENTS);
        }

        if (remaining <= 1) {
            return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
        }

        *out++ = *fmt++;
        remaining--;
        count++;
    }

    *out = '\0';
    return count;
}

template<typename T, typename... Args>
constexpr core::expected<i32, FormatError> formatImpl(char*& out, i32 remaining, const char* fmt, T value, Args... args) {
    using Type =  PlaceHolderOptions::Type;

    i32 count = 0;
    while (*fmt) {
        if (escapeBrackets(out, remaining, fmt, count)) {
            continue;
        }

        if (fmt[0] == '{') {
            fmt += 1; // Skip the bracket

            PlaceHolderOptions options = PlaceHolderOptions::parse(remaining, fmt);
            if (options.type == Type::Invalid) {
                return core::unexpected(FormatError::INVALID_PLACEHOLDER);
            }

            auto argRes = appendArg(out, remaining, value, options);
            if (argRes.hasErr()) return argRes;
            count += argRes.value();

            auto restRes = detail::formatImpl(out, remaining, fmt, args...);
            if (restRes.hasErr()) return restRes;
            count += restRes.value();

            return count;
        }

        if (remaining <= 1) {
            return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
        }

        *out++ = *fmt++;
        remaining--;
        count++;
    }

    return core::unexpected(FormatError::TOO_MANY_ARGUMENTS);
}

} // namespace detail

} // namespace core
