#pragma once

#include <API.h>
#include <types.h>

#include <string_view>
#include <string>

namespace core::error
{

using namespace coretypes;

struct CORE_API_EXPORT Error {

    Error()                     : m_msg({}) {}
    Error(std::string_view msg) : m_msg(msg) {}
    Error(const Error& other)   : m_msg(other.m_msg) {}
    Error(Error&& other)        : m_msg(std::move(other.m_msg)) {}

    Error& operator=(const Error& other) {
        m_msg = other.m_msg;
        return *this;
    }

    bool  IsErr()          const { return !m_msg.empty(); }
    const std::string& Err() const { return m_msg; }

private:
    std::string m_msg;
};

struct CORE_API_EXPORT ErrorInt {
    static constexpr i32 Ok = 0;

    constexpr ErrorInt()                      : m_code(ErrorInt::Ok) {}
    constexpr ErrorInt(i32 code)              : m_code(code) {}
    constexpr ErrorInt(const ErrorInt& other) : m_code(other.m_code) {}
    constexpr ErrorInt(ErrorInt&& other)      : m_code(std::move(other.m_code)) {}

    constexpr ErrorInt& operator=(const ErrorInt& other) {
        m_code = other.m_code;
        return *this;
    }

    bool IsErr() const { return m_code != ErrorInt::Ok; }
    i32  Err()     const { return m_code; }

    constexpr explicit operator i32() const { return m_code; }

    friend constexpr bool operator==(i32 a, ErrorInt b)      { return a == b.m_code; }
    friend constexpr bool operator==(ErrorInt a, i32 b)      { return b == a; }
    friend constexpr bool operator==(ErrorInt a, ErrorInt b) { return a.m_code == b.m_code; }

private:
    i32 m_code;
};

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
};

} // namespace core::error
