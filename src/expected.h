#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

// TODO: Including new might break the nostdlib build on some systems?
#include <new>

namespace core {

using namespace coretypes;

// NOTE: Using an unexpected_t wrapper allows the expected struct to be used with the same type for both error and value.
template <typename E>
struct CORE_API_EXPORT unexpected_t {
    E err;
    explicit unexpected_t(E&& e) : err(core::forward<E>(e)) {}
};

template <typename E>
unexpected_t<E> CORE_API_EXPORT unexpected(E&& e) { return unexpected_t<E>(core::forward<E>(e)); }

template <typename...> struct expected;

template <typename T, typename TErr>
struct CORE_API_EXPORT expected<T, TErr> {
    constexpr expected(T&& value)  : m_value(core::forward<T>(value)), m_hasValue(true) {}
    template <typename TErr2>
    expected(unexpected_t<TErr2>&& wrapper) : m_err(core::move(wrapper.err)), m_hasValue(false) {}

    // no copy
    expected(const expected&) = delete;
    expected& operator=(const expected&) = delete;

    // move
    constexpr expected(expected&& other) : m_hasValue(other.m_hasValue) {
        if (m_hasValue) new (&m_value) T(core::move(other.m_value));
        else new (&m_err) TErr(core::move(other.m_err));
    }

    ~expected() {
        if (has_value()) m_value.~T();
        else m_err.~TErr();
    }

    constexpr bool has_value() const { return m_hasValue; }
    constexpr bool has_err()   const { return !m_hasValue; }

    constexpr const T& value()  const { return m_value; }
    constexpr T& value()              { return m_value; }
    constexpr const TErr& err() const { return m_err; }
    constexpr TErr& err()             { return m_err; }

    constexpr expected<T, TErr>& check() {
        Assert(!has_err(), "expected has an error");
        return *this;
    }

private:

    union { T m_value; TErr m_err; };
    bool m_hasValue;
};

template <typename TErr>
struct CORE_API_EXPORT expected<TErr> {
    constexpr expected() : m_hasErr(false) {}
    template <typename TErr2>
    expected(unexpected_t<TErr2>&& wrapper) : m_hasErr(true), m_err(core::move(wrapper.err)) {}

    // no copy
    expected(const expected&) = delete;
    expected& operator=(const expected&) = delete;

    // move
    constexpr expected(expected&& other) : m_err(core::move(other.m_err)), m_hasErr(other.m_hasErr) {}

    constexpr const TErr& err() const { return m_err; }
    constexpr TErr& err()             { return m_err; }
    constexpr bool has_err()   const  { return m_hasErr; }

    constexpr expected<TErr>& check() {
        Assert(!has_err(), "expected has an error");
        return *this;
    }

private:
    bool m_hasErr;
    TErr m_err;
};

#define Check(expr) (expr).check()
#define ValueOrDie(expr) core::move(Check(expr).value())

#define ValueOrReturn(expr, v) {       \
    auto __ret = (expr);               \
    if (__ret.has_err()) return __ret; \
    (v) = __ret.value();               \
}

} // namespace core
