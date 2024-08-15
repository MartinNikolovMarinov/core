#pragma once

#pragma once

#include <core_assert.h>
#include <core_traits.h>
#include <core_types.h>

#include <new>
#include <utility>

// NOTE: Expected has 2 implementations for runtime and for compiletime. It's possible to use inheritance to avoid code
//       duplication, but that will have a performance cost which I am not willing to pay. This code should be optimized
//       to a simple if statement in most cases anything else is unacceptable.

namespace core {

using namespace coretypes;

// NOTE: Using an unexpected_t wrapper allows the expected struct to be used with the same type for both error and value.
template <typename E>
struct unexpected_t {
    E err;
    constexpr explicit unexpected_t(E&& e) : err(std::forward<E>(e)) {}
};

template <typename E>
constexpr unexpected_t<E> unexpected(E&& e) { return unexpected_t<E>(std::forward<E>(e)); }

template <typename...> struct expected;

template <typename T, typename TErr>
struct expected<T, TErr> {
    static_assert(std::is_standard_layout_v<T>, "type must be standard layout to store in a union");

    constexpr expected(T&& value) : m_value(std::forward<T>(value)), m_hasValue(true) {}
    template <typename TErr2>
    constexpr expected(unexpected_t<TErr2>&& wrapper) : m_err(std::move(wrapper.err)), m_hasValue(false) {}

    // no copy
    constexpr expected(const expected&) = delete;
    constexpr expected& operator=(const expected&) = delete;

    // move
    constexpr expected(expected&& other) : m_hasValue(other.m_hasValue) {
        if (m_hasValue) new (&m_value) T(std::move(other.m_value));
        else new (&m_err) TErr(std::move(other.m_err));
    }

    constexpr ~expected() {
        if constexpr (!std::is_trivially_destructible_v<T> ||
                      !std::is_trivially_destructible_v<TErr>) {
            if (hasValue()) m_value.~T();
            else m_err.~TErr();
        }
    }

    constexpr bool hasValue() const { return m_hasValue; }
    constexpr bool hasErr()   const { return !m_hasValue; }

    constexpr const T& value() const  { return m_value; }
    constexpr T& value()              { return m_value; }
    constexpr const TErr& err() const { return m_err; }
    constexpr TErr& err()             { return m_err; }

private:

    union { T m_value; TErr m_err; };
    bool m_hasValue;
};

template <typename TErr>
struct expected<TErr> {
    constexpr expected() : m_hasErr(false) {}
    template <typename TErr2>
    constexpr expected(unexpected_t<TErr2>&& wrapper) : m_hasErr(true), m_err(std::move(wrapper.err)) {}
    constexpr ~expected() = default;

    // no copy
    constexpr expected(const expected&) = delete;
    constexpr expected& operator=(const expected&) = delete;

    // move
    constexpr expected(expected&& other) : m_hasErr(other.m_hasErr), m_err(std::move(other.m_err)) {}

    constexpr const TErr& err() const { return m_err; }
    constexpr TErr& err()             { return m_err; }
    constexpr bool hasErr() const     { return m_hasErr; }

private:

    bool m_hasErr;
    TErr m_err;
};

template <typename T, typename TErr>
constexpr inline T&& Unpack(expected<T, TErr>&& exp, [[maybe_unused]] const char* msg = nullptr) {
    Panic(!exp.hasErr(), msg);
    return std::move(exp.value());
}

template <typename TErr>
constexpr inline void Expect(expected<TErr>&& expr, [[maybe_unused]] const char* msg = nullptr) {
    Panic(!expr.hasErr(), msg);
}

} // namespace core
