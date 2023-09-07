#pragma once

#include <types.h>
#include <utils.h>
#include <core_traits.h>

#include <new>

// FIXME: I should combine the except and the static except. The static-ness should be an implementation detail.

namespace core {

using namespace coretypes;

// NOTE: Using an unexpected_t wrapper allows the expected struct to be used with the same type for both error and value.
template <typename E>
struct unexpected_t {
    E err;
    constexpr explicit unexpected_t(E&& e) : err(core::forward<E>(e)) {}
};

template <typename E>
constexpr unexpected_t<E> unexpected(E&& e) { return unexpected_t<E>(core::forward<E>(e)); }

template <typename...> struct expected;

template <typename T, typename TErr>
struct expected<T, TErr> {
    // NOTE: The type must be standard layout to store it in a union.
    static_assert(core::is_standard_layout_v<T>, "type must be standard layout");

    expected(T&& value)  : m_value(core::forward<T>(value)), m_hasValue(true) {}
    template <typename TErr2>
    expected(unexpected_t<TErr2>&& wrapper) : m_err(core::move(wrapper.err)), m_hasValue(false) {}

    // no copy
    expected(const expected&) = delete;
    expected& operator=(const expected&) = delete;

    // move
    expected(expected&& other) : m_hasValue(other.m_hasValue) {
        if (m_hasValue) new (&m_value) T(core::move(other.m_value));
        else new (&m_err) TErr(core::move(other.m_err));
    }

    ~expected() {
        if (has_value()) m_value.~T();
        else m_err.~TErr();
    }

    bool has_value() const { return m_hasValue; }
    bool has_err()   const { return !m_hasValue; }

    const T& value()  const { return m_value; }
    T& value()              { return m_value; }
    const TErr& err() const { return m_err; }
    TErr& err()             { return m_err; }

    expected<T, TErr>& check(const char* msg) {
        Assert(!has_err(), msg);
        return *this;
    }

private:

    union { T m_value; TErr m_err; };
    bool m_hasValue;
};

template <typename TErr>
struct expected<TErr> {
    expected() : m_hasErr(false) {}
    template <typename TErr2>
    expected(unexpected_t<TErr2>&& wrapper) : m_hasErr(true), m_err(core::move(wrapper.err)) {}
    ~expected() = default;

    // no copy
    expected(const expected&) = delete;
    expected& operator=(const expected&) = delete;

    // move
    expected(expected&& other) : m_hasErr(other.m_hasErr), m_err(core::move(other.m_err)) {}

    const TErr& err() const { return m_err; }
    TErr& err()             { return m_err; }
    bool has_err()   const  { return m_hasErr; }

    expected<TErr>& check(const char* msg) {
        Assert(!has_err(), msg);
        return *this;
    }

private:

    bool m_hasErr;
    TErr m_err;
};

template <typename...> struct static_expected;

template <typename T, typename TErr>
struct static_expected<T, TErr> {
    // NOTE: The type must be standard layout to store it in a union.
    static_assert(core::is_standard_layout_v<T>, "type must be standard layout");
    static_assert(core::is_trivially_destructible_v<T>, "type must be trivially destructible");

    constexpr static_expected(T&& value) : m_value(core::forward<T>(value)), m_hasValue(true) {}
    template <typename TErr2>
    constexpr static_expected(unexpected_t<TErr2>&& wrapper) : m_err(core::move(wrapper.err)), m_hasValue(false) {}

    // no copy
    constexpr static_expected(const static_expected&) = delete;
    constexpr static_expected& operator=(const static_expected&) = delete;

    // move
    constexpr static_expected(static_expected&& other) : m_hasValue(other.m_hasValue) {
        if (m_hasValue) new (&m_value) T(core::move(other.m_value));
        else new (&m_err) TErr(core::move(other.m_err));
    }

    constexpr bool has_value() const { return m_hasValue; }
    constexpr bool has_err()   const { return !m_hasValue; }

    constexpr const T& value()  const { return m_value; }
    constexpr T& value()              { return m_value; }
    constexpr const TErr& err() const { return m_err; }
    constexpr TErr& err()             { return m_err; }

    constexpr static_expected<T, TErr>& check(const char* msg) {
        Assert(!has_err(), msg);
        return *this;
    }

private:

    union { T m_value; TErr m_err; };
    bool m_hasValue;
};

#ifndef Expect
    #define Expect(...) C_VFUNC(Expect, __VA_ARGS__)
    #define Expect1(expr) (expr).check("failed expectation")
    #define Expect2(expr, msg) (expr).check(msg)
#endif

#ifndef ValueOrDie
    #define ValueOrDie(...) C_VFUNC(ValueOrDie, __VA_ARGS__)
    #define ValueOrDie1(expr) core::move(Expect1(expr).value())
    #define ValueOrDie2(expr, msg) core::move(Expect2(expr, msg).value())
#endif

} // namespace core
