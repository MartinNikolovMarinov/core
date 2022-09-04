#pragma once

#include <API.h>
#include <types.h>

#include <string_view>
#include <string>

namespace core::error
{

using namespace coretypes;

struct CORE_API_EXPORT Error {
    std::string msg;

    Error() : msg(nullptr) {}
    Error(std::string_view msg) : msg(msg) {}
    Error(const Error& other) : msg(other.msg) {}
    Error(Error&& other) : msg(std::move(other.msg)) {}

    Error& operator=(const Error& other) {
        msg = other.msg;
        return *this;
    }

    bool IsErr()             const { return !msg.empty(); }
    const std::string& Err() const { return msg; }
};

struct CORE_API_EXPORT ErrorInt {
    static constexpr i32 Ok = 0;

    i32 code;

    constexpr ErrorInt() : code(ErrorInt::Ok) {}
    constexpr ErrorInt(i32 code) : code(code) {}
    constexpr ErrorInt(const ErrorInt& other) : code(other.code) {}
    constexpr ErrorInt(ErrorInt&& other) : code(std::move(other.code)) {}

    constexpr ErrorInt& operator=(const ErrorInt& other) {
        code = other.code;
        return *this;
    }

    constexpr bool IsErr() const { return code != ErrorInt::Ok; }
    constexpr i32  Err()   const { return code; }

    constexpr explicit operator i32() const { return code; }
};

constexpr bool operator==(i32 a, ErrorInt b)      { return a == b.code; }
constexpr bool operator==(ErrorInt a, i32 b)      { return b == a; }
constexpr bool operator==(ErrorInt a, ErrorInt b) { return a.code == b.code; }

template<typename T, typename TErr>
struct CORE_API_EXPORT ErrorValue {
    T val;
    TErr err;

    ErrorValue() = default;
    ErrorValue(T&& val, TErr&& err) : val(std::move(val)), err(std::move(err)) {}
    ErrorValue(const ErrorValue& other) : val(other.val), err(other.err) {}
    ErrorValue(ErrorValue&& other) : val(std::move(other.val)), err(std::move(other.err)) {}

    ErrorValue& operator=(const ErrorValue& other) {
        val = other.val;
        err = other.err;
        return *this;
    }

    bool  IsErr()     const { return err.IsErr(); }
    const TErr& Err() const { return err; }
};

} // namespace core::error
