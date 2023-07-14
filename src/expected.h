#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>
#include <core_traits.h>

// TODO: Including new might break the nostdlib build on some systems?
#include <new>

namespace core {

using namespace coretypes;

// NOTE: Using an unexpected_t wrapper allows the expected struct to be used with the same type for both error and value.
template <typename E>
struct CORE_API_EXPORT unexpected_t {
    E err;
    constexpr explicit unexpected_t(E&& e) : err(core::forward<E>(e)) {}
};

template <typename E>
constexpr unexpected_t<E> CORE_API_EXPORT unexpected(E&& e) { return unexpected_t<E>(core::forward<E>(e)); }

template <typename...> struct expected;

template <typename T, typename TErr>
struct CORE_API_EXPORT expected<T, TErr> {
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

    expected<T, TErr>& check() {
        Assert(!has_err(), "expected has an error");
        return *this;
    }

private:

    union { T m_value; TErr m_err; };
    bool m_hasValue;
};

template <typename TErr>
struct CORE_API_EXPORT expected<TErr> {
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

    expected<TErr>& check() {
        Assert(!has_err(), "expected has an error");
        return *this;
    }

private:
    bool m_hasErr;
    TErr m_err;
};

template <typename...> struct static_expected;

// IMPORTANT: Do not put anything which is not trivially destructible in this struct!
template <typename T, typename TErr>
struct CORE_API_EXPORT static_expected<T, TErr> {
    static_assert(core::IsTrivial_v<T>, "trivial type is required");

    constexpr static_expected(T&& value)  : m_value(core::forward<T>(value)), m_hasValue(true) {}
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

    constexpr static_expected<T, TErr>& check() {
        Assert(!has_err(), "expected has an error");
        return *this;
    }

private:

    union { T m_value; TErr m_err; };
    bool m_hasValue;
};

#ifndef Check
    #define Check(expr) (expr).check()
#endif

#ifndef ValueOrDie
    #define ValueOrDie(expr) core::move(Check(expr).value())
#endif

#ifndef ValueOrReturn
    #define ValueOrReturn(...) C_VFUNC(ValueOrReturn, __VA_ARGS__)
    #define ValueOrReturn1(expr) {         \
        auto __ret = (expr);               \
        if (__ret.has_err()) return __ret; \
    }
    #define ValueOrReturn2(expr, v) {      \
        auto __ret = (expr);               \
        if (__ret.has_err()) return __ret; \
        (v) = __ret.value();               \
    }
#endif

} // namespace core
