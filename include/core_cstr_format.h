#pragma once

#include <core_cstr_conv.h>
#include <core_expected.h>
#include <core_str_view.h>
#include <core_types.h>

// TODO:
// Things to finish:
//   0. print for char types!
//   1. Replace all places where std::snprintf is used. Where it makes sense only.

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

constexpr const char* formatErrorToCStr(FormatError err) {
    switch(err) {
        case FormatError::INVALID_ARGUMENTS:   return "Invalid arguments";
        case FormatError::TOO_FEW_ARGUMENTS:   return "Too few arguments";
        case FormatError::TOO_MANY_ARGUMENTS:  return "Too many arguments";
        case FormatError::OUT_BUFFER_OVERFLOW: return "Out buffer overflow";
        case FormatError::INVALID_PLACEHOLDER: return "Invalid placeholder";

        case FormatError::SENTINEL: [[fallthrough]];
        default: break;
    }

    return "unknown";
}

constexpr core::expected<i32, FormatError> format(char* out, i32 outLen, const char* fmt);
template<typename... Args>
constexpr core::expected<i32, FormatError> format(char* out, i32 outLen, const char* fmt, Args... args);
template<typename... Args>
constexpr core::expected<i32, FormatError> format(core::Memory<char> out, const char* fmt, Args... args);

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

    static constexpr PlaceHolderOptions parse(const char*& fmt) {
        PlaceHolderOptions options;
        options.type = Type::Invalid;
        options.padding = 0;
        options.paddingSymbol = '\0';

        if (*fmt == '}') [[likely]] {
            // This will be the most likely path for most placeholders.
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
        constexpr u8 BUFF_SIZE = 5;
        char buff[BUFF_SIZE + 1] = {};
        u8 buffWriteIdx = 0;
        Type deducedType = Type::Invalid;

        // Example formatting:
        // {}, {03:}, {03:h}, {03:H}, {03:b}, {03:B}, {04:f.3}

        for (; *fmt; fmt++) {
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
                if (deducedType == Type::FixedFloat) {
                    auto res = core::cstrToInt<u16>(buff, u32(core::cstrLen(buff)));
                    Assert(res.hasValue(), "This problem should have been caught earlier.");
                    if (res.hasErr()) break;
                    options.fixedFloat.fixed = res.value();
                }

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

constexpr core::expected<i32, FormatError> prependPadding(char*& out, i32 outLen, i32 padding, char paddingSymbol, i32 writeTarget) {
    i32 repeatCount = padding - writeTarget;
    if (repeatCount > 0) {
        if (outLen <= repeatCount) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
        out += core::memset(out, paddingSymbol, addr_size(repeatCount));
        return repeatCount;
    }
    return 0;
}

template <typename T>
constexpr core::expected<i32, FormatError> convertInts(char* out, i32 outLen, T value, PlaceHolderOptions& options) {
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

            return written;
        }

        case PlaceHolderOptions::Type::PaddingOnly: {
            i32 writeTarget = i32(core::digitCount(value));

            auto prependRes = prependPadding(out, outLen, options.padding, options.paddingSymbol, writeTarget);
            if (prependRes.hasErr()) return core::unexpected(prependRes.err());
            i32 prependedLen = prependRes.value();
            if (outLen <= prependedLen + writeTarget) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

            auto intToCstrRes = core::intToCstr(value, out, u32(outLen), u32(writeTarget));
            if (intToCstrRes.hasErr()) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }
            Assert(intToCstrRes.value() == u32(writeTarget), "Sanity check failed");

            i32 written = prependedLen + writeTarget;
            return written;
        }

        case PlaceHolderOptions::Type::Hex: {
            constexpr i32 fullHexLen = i32(sizeof(T) << 1); // space for one hex element
            i32 lz = i32(core::intrin_countLeadingZeros(value));
            i32 leadingHexZeros = lz / 4;
            i32 writeTarget = core::core_max(fullHexLen - leadingHexZeros, 1);

            auto prependRes = prependPadding(out, outLen, options.padding, options.paddingSymbol, writeTarget);
            if (prependRes.hasErr()) return core::unexpected(prependRes.err());
            i32 prependedLen = prependRes.value();
            if (outLen <= prependedLen + writeTarget) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

            u32 hexLen = u32(writeTarget);
            auto intToHexRes = core::intToHex(value, out, addr_size(writeTarget + 1), options.hex.upperCase, hexLen);
            if (intToHexRes.hasErr()) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }
            Assert(intToHexRes.value() == u32(writeTarget), "Sanity check failed");

            i32 written = prependedLen + writeTarget;
            return written;
        }

        case PlaceHolderOptions::Type::Binary: {
            // space for the binary element without leading zeroes.
            constexpr i32 bytes = sizeof(T) * core::BYTE_SIZE;
            i32 lz = i32(core::intrin_countLeadingZeros(value));
            i32 writeTarget = core::core_max(bytes - lz, 1);

            auto prependRes = prependPadding(out, outLen, options.padding, options.paddingSymbol, writeTarget);
            if (prependRes.hasErr()) return core::unexpected(prependRes.err());
            i32 prependedLen = prependRes.value();
            if (outLen <= prependedLen + writeTarget) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

            u32 binaryLength = u32(writeTarget);
            auto intToBinaryRes = core::intToBinary(value, out, addr_size(writeTarget + 1), binaryLength);
            if (intToBinaryRes.hasErr()) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }
            Assert(intToBinaryRes.value() == u32(writeTarget), "Sanity check failed");

            i32 written = prependedLen + writeTarget;
            return written;
        }

        case PlaceHolderOptions::Type::Invalid:     [[fallthrough]];
        case PlaceHolderOptions::Type::FixedFloat:  [[fallthrough]];
        default:                                    break;
    }

    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
}

template <typename T>
constexpr core::expected<i32, FormatError> convertFloats(char* out, i32 outLen, T value, PlaceHolderOptions& options) {
    switch (options.type) {
        case PlaceHolderOptions::Type::Empty: {
            auto res = core::floatToCstr(value, out, u32(outLen));
            if (res.hasErr()) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }

            auto written = i32(res.value());
            if (written >= outLen) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }

            return written;
        }

        case PlaceHolderOptions::Type::FixedFloat: {
            if constexpr (sizeof(T) < 8) {
                // Fixed floating point printing is supported only for f64
                // I can't seem to make static assertion work here. Oh well.
                return core::unexpected(FormatError::INVALID_PLACEHOLDER);
            }
            else {
                if (options.padding > 0) {
                    // Padding is not allowed with fixed precision because the the output string could be fairly large and
                    // there is no easy way to
                    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
                }

                u32 precision = options.fixedFloat.fixed;
                auto res = core::floatToFixedCstr(f64(value), precision, out, u32(outLen));
                if (res.hasErr()) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

                i32 written = i32(res.value());
                if (written >= outLen) {
                    // TODO2: [BUG]
                    // In few cases float to fixed writes to the last byte which should be reserved for the null terminator.
                    // Technically that is a bug, but it's hard to find and I have not seen it read pass the end of the buffer.
                    // Valgrind check confirms that. If there is uninitialised memory access I should dig deeper.
                    return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
                }

                return written;
            }
        }

        case PlaceHolderOptions::Type::PaddingOnly: [[fallthrough]];
        case PlaceHolderOptions::Type::Hex:         [[fallthrough]];
        case PlaceHolderOptions::Type::Binary:      [[fallthrough]];
        case PlaceHolderOptions::Type::Invalid:     [[fallthrough]];
        default:                                    break;
    }

    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
}

constexpr core::expected<i32, FormatError> convertStrs(char* out, i32 outLen, const char* value, i32 vlen, PlaceHolderOptions& options) {
    switch (options.type) {
        case PlaceHolderOptions::Type::Empty: {
            if (vlen <= 0) return 0;

            if (outLen <= vlen) {
                return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);
            }
            core::memcopy(out, value, addr_size(vlen));
            return vlen;
        }

        case PlaceHolderOptions::Type::PaddingOnly: {
            if (vlen <= 0) return 0;

            i32 writeTarget = vlen;

            auto prependRes = prependPadding(out, outLen, options.padding, options.paddingSymbol, writeTarget);
            if (prependRes.hasErr()) return core::unexpected(prependRes.err());
            i32 prependedLen = prependRes.value();
            if (outLen <= prependedLen + writeTarget) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

            core::memcopy(out, value, addr_size(writeTarget));
            i32 written = prependedLen + writeTarget;
            return written;
        }

        case PlaceHolderOptions::Type::Hex: {
            if (vlen <= 0) return 0;
            if (options.padding > 0) {
                // Padding is not allowed
                return core::unexpected(FormatError::INVALID_PLACEHOLDER);
            }

            i32 spacesBetweenElements = vlen * i32(sizeof(' ')) - 1; // -1 because the last element does not put a space.
            i32 spaceForOneHexElement = i32(sizeof(u8) << 1);
            i32 totalSize = spaceForOneHexElement * vlen + spacesBetweenElements;
            [[maybe_unused]] i32 sanityCheckSize = 0;

            if (outLen <= totalSize) {
                return core::unexpected(core::FormatError::OUT_BUFFER_OVERFLOW);
            }

            for (i32 i = 0; i < vlen; i++) {
                u8 curr = u8(value[i]);
                auto toHexRes = core::intToHex(curr, out, addr_size(spaceForOneHexElement + 1), options.hex.upperCase);
                if (toHexRes.hasErr()) [[unlikely]] {
                    // This should have been detected before the loop starts!
                    return core::unexpected(core::FormatError::OUT_BUFFER_OVERFLOW);
                }
                out += toHexRes.value();
                if (i != vlen - 1) *out++ = ' ';

                #if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED == 1
                    sanityCheckSize += toHexRes.value();
                    if (i != vlen - 1) sanityCheckSize++;
                #endif
            }

            Assert(sanityCheckSize == totalSize);
            return totalSize;
        }

        case PlaceHolderOptions::Type::Binary: {
            if (vlen <= 0) return 0;
            if (options.padding > 0) {
                // Padding is not allowed
                return core::unexpected(FormatError::INVALID_PLACEHOLDER);
            }

            i32 spacesBetweenElements = vlen * i32(sizeof(' ')) - 1; // -1 because the last element does not put a space.
            i32 spaceForOneBinaryElement = core::BYTE_SIZE;
            i32 totalSize = spaceForOneBinaryElement * vlen + spacesBetweenElements;
            [[maybe_unused]] i32 sanityCheckSize = 0;

            if (outLen <= totalSize) {
                return core::unexpected(core::FormatError::OUT_BUFFER_OVERFLOW);
            }

            for (i32 i = 0; i < vlen; i++) {
                u8 curr = u8(value[i]);
                auto toBinRes = core::intToBinary(curr, out, addr_size(spaceForOneBinaryElement + 1));
                if (toBinRes.hasErr()) [[unlikely]] {
                    // This should have been detected before the loop starts!
                    return core::unexpected(core::FormatError::OUT_BUFFER_OVERFLOW);
                }
                out += toBinRes.value();
                if (i != vlen - 1) *out++ = ' ';

                #if defined(CORE_ASSERT_ENABLED) && CORE_ASSERT_ENABLED == 1
                    sanityCheckSize += toBinRes.value();
                    if (i != vlen - 1) sanityCheckSize++;
                #endif
            }

            Assert(sanityCheckSize == totalSize);
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
    if (outLen <= 1) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

    switch (options.type) {
        case PlaceHolderOptions::Type::PaddingOnly: [[fallthrough]];
        case PlaceHolderOptions::Type::Empty: {
            if (value) {
                return convertToCStr(out, outLen, "true"_sv, options);
            }
            return convertToCStr(out, outLen, "false"_sv, options);

            // NOTE: Using ternary operators in consteval context with MSVC is crazy.
            //
            // The following is not valid compile-time constant expression according to MSVC 19.29.30136.0
            // return value ? convertToCStr(out, outLen, "true"_sv, options)
            //              : convertToCStr(out, outLen, "false"_sv, options);
            //
            // Error Message:
            //    error C2975: 'V': invalid template argument for 'core::force_consteval', expected compile-time constant expression
            //
            // But it is the exact same code! WTF...
            //
            // Possible reason:
            // The ternary operator requires both its true and false expressions to be valid compile-time constants when
            // used in a consteval context. So that creates a problem somehow, EVEN THOUGH both branches are valid,
            // and are both tested.
        }

        case PlaceHolderOptions::Type::Binary: [[fallthrough]];
        case PlaceHolderOptions::Type::Hex: {
            constexpr i32 writeTarget = 1; // 1 for 0 or 1

            auto prependRes = prependPadding(out, outLen, options.padding, options.paddingSymbol, writeTarget);
            if (prependRes.hasErr()) return core::unexpected(prependRes.err());
            i32 prependedLen = prependRes.value();
            if (outLen <= prependedLen + writeTarget) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

            *out = value ? '1' : '0';
            i32 written = prependedLen + writeTarget;
            return written;
        }

        case PlaceHolderOptions::Type::FixedFloat: [[fallthrough]];
        case PlaceHolderOptions::Type::Invalid:    [[fallthrough]];
        default: break;
    }

    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
}

constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, char value, PlaceHolderOptions& options) {
    switch (options.type) {
        case PlaceHolderOptions::Type::Empty: {
            constexpr i32 writeTarget = 1;

            auto prependRes = prependPadding(out, outLen, options.padding, options.paddingSymbol, writeTarget);
            if (prependRes.hasErr()) return core::unexpected(prependRes.err());
            i32 prependedLen = prependRes.value();
            if (outLen <= prependedLen + writeTarget) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

            *out = value;
            i32 written = prependedLen + writeTarget;
            return written;
        }

        case PlaceHolderOptions::Type::Binary: [[fallthrough]];
        case PlaceHolderOptions::Type::Hex:
            return convertInts(out, outLen, u8(value), options);

        case PlaceHolderOptions::Type::PaddingOnly: [[fallthrough]];
        case PlaceHolderOptions::Type::FixedFloat:  [[fallthrough]];
        case PlaceHolderOptions::Type::Invalid:     [[fallthrough]];
        default: break;
    }

    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
}

constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, f32 value, PlaceHolderOptions& options) { return convertFloats(out, outLen, value, options); }
constexpr core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, f64 value, PlaceHolderOptions& options) { return convertFloats(out, outLen, value, options); }

template <typename T>
core::expected<i32, FormatError> convertToCStr(char* out, i32 outLen, T ptr, PlaceHolderOptions& options) {
    // ------------------------------------- DEFAULT CONVERSION FUNCTION -----------------------------------------------

    if constexpr (std::is_pointer_v<T> || std::is_same_v<T, std::nullptr_t>) {
        // Only for pointers or nullptr literals.

        if (ptr == nullptr) {
            return convertToCStr(out, outLen, "null", options);
        }

        switch (options.type) {
            case PlaceHolderOptions::Type::Empty:  [[fallthrough]];
            case PlaceHolderOptions::Type::Hex:    [[fallthrough]];
            case PlaceHolderOptions::Type::Binary:
                // How unfortunate that pointers can't be bitcasted to integers. Oh well, reinterpret_cast it is.
                return convertInts(out, outLen, reinterpret_cast<addr_size>(ptr), options);

            case PlaceHolderOptions::Type::PaddingOnly: [[fallthrough]];
            case PlaceHolderOptions::Type::Invalid:     [[fallthrough]];
            case PlaceHolderOptions::Type::FixedFloat:  [[fallthrough]];
            default:                                    return core::unexpected(FormatError::INVALID_PLACEHOLDER);
        }
    }
    else {
        return core::unexpected(FormatError::INVALID_ARGUMENTS);
    }
}

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

        if (*fmt == '{') {
            // NOTE:
            // An opening bracket here means that there is not corresponding argument. This is definitely an error, but
            // what kind of an error needs to be determined.
            // This is slow, but it is executed only once on the slow path.

            bool foundAClosingBracket = false;
            while (*fmt) {
                if (*fmt == '}') foundAClosingBracket = true;
                fmt++;
            }

            if (foundAClosingBracket)
                return core::unexpected(FormatError::TOO_FEW_ARGUMENTS);
            else
                return core::unexpected(FormatError::INVALID_PLACEHOLDER);
        }
        if (*fmt == '}') {
            // This indicates a floating closing bracket without an opening bracket.
            return core::unexpected(FormatError::INVALID_PLACEHOLDER);
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

            if (remaining <= 1) return core::unexpected(FormatError::OUT_BUFFER_OVERFLOW);

            PlaceHolderOptions options = PlaceHolderOptions::parse(fmt);
            if (options.type == Type::Invalid) {
                return core::unexpected(FormatError::INVALID_PLACEHOLDER);
            }

            auto argRes = appendArg(out, remaining, value, options);
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
        remaining--;
        count++;
    }

    return core::unexpected(FormatError::TOO_MANY_ARGUMENTS);
}

} // namespace detail

} // namespace core
