#pragma once

#include <API.h>
#include <types.h>
#include <mem.h>
#include <utils.h>

namespace core {

using namespace coretypes;

template <typename T, core::ptr_size N>
struct CORE_API_EXPORT sarr {
    using data_type = T;
    using size_type = ptr_size;

    static constexpr bool dataIsTrivial = core::IsTrivial_v<data_type>;
    static_assert(dataIsTrivial, "sarr only supports trivial types");

    constexpr sarr() : m_data(), m_len(0) {}

    constexpr size_type cap()      const { return N; }
    constexpr size_type len()      const { return m_len; }
    constexpr size_type byte_len() const { return m_len * sizeof(data_type); }
    constexpr data_type* data()    const { return m_data; }
    constexpr bool empty()         const { return m_len == 0; }
    constexpr void clear()               { m_len = 0; }

    constexpr sarr<T, N> copy() {
        sarr<T, N> result(*this);
        return result;
    }

    constexpr data_type& at(size_type idx)                     { return m_data[idx]; }
    constexpr const data_type& at(size_type idx)         const { return m_data[idx]; }
    constexpr data_type& operator[](size_type idx)             { return at(idx); }
    constexpr const data_type& operator[](size_type idx) const { return at(idx); }

    constexpr data_type& first()             { return at(0); }
    constexpr const data_type& first() const { return at(0); }
    constexpr data_type& last()              { return at(m_len - 1); }
    constexpr const data_type& last()  const { return at(m_len - 1); }

    constexpr sarr<T, N>& append(const data_type& val) {
        m_data[m_len] = val;
        m_len++;
        return *this;
    }

    constexpr sarr<T, N>& append(data_type&& val) {
        m_data[m_len] = core::move(val);
        m_len++;
        return *this;
    }

    constexpr sarr<T, N>& append(const sarr<T, N>& other) {
        for (size_type i = 0; i < other.m_len; ++i) append(other.m_data[i]);
        return *this;
    }

    constexpr sarr<T, N>& fill(const data_type& val) {
        for (size_type i = 0; i < m_len; ++i) m_data[i] = val;
        return *this;
    }

private:
    data_type m_data[N];
    size_type m_len;
};

namespace detail {

template<typename TArg, typename ...Args>
constexpr CORE_API_EXPORT auto _create_sarr(TArg first, Args... rest) {
    auto ret = sarr<TArg, (sizeof...(rest) + 1)>();
    ret.append(first);
    auto f = [&](auto arg) { ret.append(arg); };
    (f(rest), ...);
    return ret;
}

};

template<typename ...Args>
constexpr CORE_API_EXPORT auto create_sarr(Args... args) {
    static_assert(sizeof...(Args) > 0, "create_sarr requires at least one argument");
    return detail::_create_sarr(args...);
}

} // namespace core
