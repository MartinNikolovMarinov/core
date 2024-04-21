#pragma once

#include <core_mem.h>
#include <core_types.h>
#include <core_exec_ctx.h>
#include <math/core_math.h>

#include <new>

namespace core {

using namespace coretypes;

template <typename T>
struct ArrList {
    using value_type = T;
    using size_type = addr_size;

    static constexpr bool dataIsTrivial = std::is_trivial_v<value_type>;

    ArrList() : m_data(nullptr), m_cap(0), m_len(0) {}
    ArrList(size_type cap) : m_data(nullptr), m_cap(cap), m_len(0) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
        }
    }
    ArrList(size_type len, const T& v) : m_data(nullptr), m_cap(len), m_len(len) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
            if constexpr (dataIsTrivial) {
                core::memfill(m_data, m_len, v);
            }
            else {
                for (size_type i = 0; i < m_len; i++) {
                    new (&m_data[i]) T(v);
                }
            }
        }
    }

    ArrList(const ArrList& other) = delete; // prevent copy ctor
    ArrList(ArrList&& other) {
        if (this == &other) return;

        m_data = other.m_data;
        m_cap = other.m_cap;
        m_len = other.m_len;

        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    ~ArrList() { free(); }

    ArrList& operator=(const ArrList& other) = delete; // prevent copy assignment
    ArrList& operator=(size_type) = delete; // prevent size assignment
    ArrList& operator=(ArrList&& other) {
        if (this == &other) return *this;

        free(); // this object was already created, so we need to free it first.

        m_data = other.m_data;
        m_cap = other.m_cap;
        m_len = other.m_len;

        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;

        return *this;
    }

    size_type         cap()     const { return m_cap; }
    size_type         byteCap() const { return m_cap * sizeof(value_type); }
    size_type         len()     const { return m_len; }
    size_type         byteLen() const { return m_len * sizeof(value_type); }
    value_type*       data()          { return m_data; }
    const value_type* data()    const { return m_data; }
    bool              empty()   const { return m_len == 0; }

    value_type&       at(size_type idx)               { return m_data[idx]; }
    const value_type& at(size_type idx)         const { return m_data[idx]; }
    value_type&       operator[](size_type idx)       { return at(idx); }
    const value_type& operator[](size_type idx) const { return at(idx); }

    value_type&       first()       { return at(0); }
    const value_type& first() const { return at(0); }
    value_type&       last()        { return at(m_len - 1); }
    const value_type& last()  const { return at(m_len - 1); }

    void clear() {
        for (size_type i = 0; i < m_len; ++i) {
            m_data[i].~T();
        }
        m_len = 0;
    }

    ArrList copy() {
        value_type* dataCopy = nullptr;
        if (m_cap > 0) {
            dataCopy = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
            if constexpr (dataIsTrivial) {
                core::memcopy(dataCopy, m_data, m_len * sizeof(value_type));
            }
            else {
                for (size_type i = 0; i < m_len; i++) {
                    new (&dataCopy[i]) T(m_data[i]);
                }
            }
        }
        ArrList result;
        result.m_data = dataCopy;
        result.m_cap = m_cap;
        result.m_len = m_len;
        return result;
    }

    void free() {
        if (m_data == nullptr) return;
        clear();
        core::free(m_data, m_cap, sizeof(value_type));
        m_cap = 0;
        m_data = nullptr;
    }

    // This is the "I know what I am doing" method. Gives ownership of data to the array. The array's destructor will
    // free it, so if a different allocator was used to allocate the data the results are undefined.
    void reset(value_type* data, size_type len) {
        free();
        m_data = data;
        m_cap = len;
        m_len = len;
    }

    value_type* release(size_type& len, size_type& cap) {
        if constexpr (!dataIsTrivial) {
            Panic(false, "Releasing the ownership of a raw pointer, of non trivial type, is strongly discouraged."
                         "Because you have to remember to manually call the destructor for each element.");
        }

        value_type* res = m_data;
        cap = m_cap;
        len = m_len;
        m_data = nullptr;
        m_cap = 0;
        m_len = 0;
        return res;
    }

    void ensureCap(size_type newCap) {
        if (newCap <= m_cap) {
            return;
        }

        // reallocate
        value_type* newData = reinterpret_cast<value_type *>(core::alloc(newCap, sizeof(value_type)));
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len * sizeof(value_type));
            core::free(m_data, m_cap, sizeof(value_type));
        }

        m_data = newData;
        m_cap = newCap;
    }

    void push(const value_type& val) {
        if (m_len == m_cap) {
            ensureCap(m_cap == 0 ? 1 : m_cap * 2);
        }

        new (&m_data[m_len]) T(val);
        m_len++;
    }

    void push(value_type&& val) {
        if (m_len == m_cap) {
            ensureCap(m_cap == 0 ? 1 : m_cap * 2);
        }

        new (&m_data[m_len]) T(std::move(val));
        m_len++;
    }

    void push(const value_type* val, size_type len) {
        if (m_len + len > m_cap) {
            // This strategy might be a bit to over eager with the size of the new capacity.
            ensureCap(m_cap <= len ? (m_cap*2 + len) : m_cap * 2);
        }

        if constexpr (dataIsTrivial) {
            core::memcopy(m_data + m_len, val, len * sizeof(value_type));
        }
        else {
            for (size_type i = 0; i < len; i++) {
                new (&m_data[i + m_len]) T(val[i]);
            }
        }

        m_len += len;
    }

    void remove(size_type idx) {
        m_data[idx].~T();

        if (idx < m_len - 1) {
            for (size_type i = idx; i < m_len - 1; ++i) {
                m_data[i] = std::move(m_data[i + 1]);
            }
        }

        m_len--;
    }

private:
    value_type* m_data;
    size_type m_cap;
    size_type m_len;
};

template <typename T, addr_size N>
struct ArrStatic {
    using value_type = T;
    using size_type = addr_size;

    static constexpr bool dataIsTrivial = std::is_trivial_v<value_type>;

    static_assert(std::is_standard_layout_v<T>, "ArrStatic should be standard layout");
    static_assert(std::is_trivially_copy_assignable_v<T>, "ArrStatic should be trivially copy assignable");
    static_assert(std::is_trivially_move_assignable_v<T>, "ArrStatic should be trivially move assignable");

    constexpr ArrStatic() : m_len(0) {}
    constexpr ArrStatic(const ArrStatic& other) = default;
    constexpr ArrStatic(ArrStatic&& other) = default;
    constexpr ArrStatic(size_type len, const T& v) : m_len(len) {
        for (size_type i = 0; i < m_len; i++) {
            m_data[i] = v;
        }
    }

    constexpr ArrStatic& operator=(const ArrStatic& other) = default;
    constexpr ArrStatic& operator=(ArrStatic&& other) = default;

    constexpr static size_type  cap()           { return N; }
    constexpr static size_type  byteCap()       { return N * sizeof(value_type); }
    constexpr size_type         len()     const { return m_len; }
    constexpr size_type         byteLen() const { return m_len * sizeof(value_type); }
    constexpr value_type*       data()          { return m_data; }
    constexpr const value_type* data()    const { return m_data; }
    constexpr bool              empty()   const { return m_len == 0; }
    constexpr void              clear()         { m_len = 0; }

    constexpr value_type&       at(size_type idx)               { return m_data[idx]; }
    constexpr const value_type& at(size_type idx)         const { return m_data[idx]; }
    constexpr value_type&       operator[](size_type idx)       { return at(idx); }
    constexpr const value_type& operator[](size_type idx) const { return at(idx); }

    constexpr value_type&       first()       { return at(0); }
    constexpr const value_type& first() const { return at(0); }
    constexpr value_type&       last()        { return at(m_len - 1); }
    constexpr const value_type& last()  const { return at(m_len - 1); }

    constexpr void push(const value_type& val) {
        m_data[m_len] = val;
        m_len++;
    }

    constexpr void push(value_type&& val) {
        m_data[m_len] = std::move(val);
        m_len++;
    }

    constexpr void push(const value_type* val, size_type len) {
        for (size_type i = 0; i < len; i++) {
            m_data[i + m_len] = val[i];
        }
        m_len += len;
    }

    constexpr void remove(size_type idx) {
        if (idx < m_len - 1) {
            for (size_type i = idx; i < m_len - 1; ++i) {
                m_data[i] = std::move(m_data[i + 1]);
            }
        }

        m_len--;
    }

private:
    value_type m_data[N];
    size_type m_len;
};

static_assert(std::is_standard_layout_v<ArrStatic<i32, 3>>, "ArrStatic should be trivial");

namespace detail {

template<typename TArg, typename ...Args>
constexpr auto _createArrStatic(TArg first, Args... rest) {
    auto ret = ArrStatic<TArg, (sizeof...(rest) + 1)>();
    ret.push(first);
    auto f = [&](auto arg) { ret.push(arg); };
    (f(rest), ...);
    return ret;
}

};

template<typename ...Args>
constexpr auto createArrStatic(Args... args) {
    static_assert(sizeof...(Args) > 0, "createArrStatic requires at least one argument");
    return detail::_createArrStatic(args...);
}

} // namespace core
