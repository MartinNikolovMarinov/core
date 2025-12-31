#pragma once

#include <core_exec_ctx.h>
#include <core_mem.h>
#include <core_traits.h>
#include <core_types.h>

#include <math/core_math.h>

namespace core {

template <typename T, AllocatorId TAllocId = DEFAULT_ALLOCATOR_ID> struct ArrList;
template <typename T, addr_size N>                                 struct ArrStatic;

template<typename ...Args> constexpr auto createArrStatic(Args... args);

template <typename From, typename To,
        typename = std::enable_if_t<std::is_rvalue_reference_v<From&&>>>
inline void convArrList(To& to, From&& from) {
    using from_value_type = typename From::value_type;
    using to_value_type = typename To::value_type;
    using size_type = typename To::size_type;

    // Validate size compatibility
    static_assert(sizeof(from_value_type) % sizeof(to_value_type) == 0 ||
                sizeof(to_value_type) % sizeof(from_value_type) == 0,
                "The size of From's and To's value types must be compatible (i.e. divisible)");

    size_type len, cap;
    from_value_type* rawData = from.release(len, cap);

    // Alignment check
    Assert(reinterpret_cast<addr_size>(rawData) % alignof(to_value_type) == 0,
        "Data alignment mismatch: From's value type is not properly aligned for To's value type");

    // Compute new length and capacity for the target type
    addr_size totalBytesLen = len * sizeof(from_value_type);
    addr_size totalBytesCap = cap * sizeof(from_value_type);

    Assert(totalBytesLen % sizeof(to_value_type) == 0,
        "Length in bytes is not divisible by To's value type size");
    Assert(totalBytesCap % sizeof(to_value_type) == 0,
        "Capacity in bytes is not divisible by To's value type size");

    size_type toLen = totalBytesLen / sizeof(to_value_type);
    size_type toCap = totalBytesCap / sizeof(to_value_type);

    // Cast raw data to the target value type
    to_value_type* castData = reinterpret_cast<to_value_type*>(rawData);

    // Transfer ownership
    to.reset(&castData, toLen, toCap);
}

#define CORE_C_ARRLEN(x) sizeof(x) / sizeof(x[0])

template <typename T, AllocatorId TAllocId>
struct ArrList {
    // NOTE: If T is not copy constructable some functionalities can't be used.
    static_assert(std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>,
                "The provided T does not satisfy the requirements for ArrList");

    inline static core::AllocatorContext& allocator = core::getAllocator(TAllocId);

    using value_type = T;
    using size_type = addr_size;

    static constexpr bool dataIsTrivial = std::is_trivial_v<value_type>;

    ArrList() : m_data(nullptr), m_cap(0), m_len(0) {}
    ArrList(size_type cap) : m_data(nullptr), m_cap(cap), m_len(0) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<value_type *>(allocator.alloc(m_cap, sizeof(value_type)));
        }
    }
    ArrList(size_type len, const T& v) : m_data(nullptr), m_cap(len), m_len(len) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<value_type *>(allocator.alloc(m_cap, sizeof(value_type)));
            if constexpr (dataIsTrivial) {
                core::memset(m_data, v, m_len);
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

    core::Memory<const value_type> memView() const { return { this->data(), this->len() }; }
    core::Memory<value_type>       mem()           { return { this->data(), this->len() }; }

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

    ArrList copy() const {
        value_type* dataCopy = nullptr;
        if (m_cap > 0) {
            dataCopy = reinterpret_cast<value_type *>(allocator.alloc(m_cap, sizeof(value_type)));
            if constexpr (dataIsTrivial) {
                core::memcopy(dataCopy, m_data, m_len);
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

        allocator.free(m_data, m_cap, sizeof(value_type));
        m_cap = 0;
        m_data = nullptr;
    }

    // This is the "I know what I am doing" method. Gives ownership of data to the array. The array's destructor will
    // free it, so if a different allocator was used to allocate the data the results are undefined.
    void reset(value_type** data, size_type len, size_type cap) {
        free();

        if (data) {
            m_data = reinterpret_cast<value_type *>(*data);
            *data = nullptr;
        }
        else {
            m_data = nullptr;
        }

        m_cap = cap;
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

        value_type* newData = reinterpret_cast<value_type *>(allocator.alloc(newCap, sizeof(value_type)));
        if (m_data != nullptr) {
            if constexpr (dataIsTrivial) {
                core::memcopy(newData, m_data, m_len);
            }
            else {
                for (size_type i = 0; i < m_len; ++i) {
                    if constexpr (std::is_move_constructible_v<T>) {
                        new (&newData[i]) T(std::move(m_data[i]));
                    }
                    else {
                        static_assert(std::is_copy_constructible_v<T>);
                        new (&newData[i]) T(m_data[i]);
                    }
                    m_data[i].~T();
                }
            }
            allocator.free(m_data, m_cap, sizeof(value_type));
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
            core::memcopy(m_data + m_len, val, len);
        }
        else {
            for (size_type i = 0; i < len; i++) {
                new (&m_data[i + m_len]) T(val[i]);
            }
        }

        m_len += len;
    }

    void push(core::Memory<const value_type> val) { push(val.data(), val.len()); }
    void push(core::Memory<value_type> val) { push(val.data(), val.len()); }

    void remove(size_type idx) {
        Assert(idx < m_len, "remove idx out of bounds");
        m_data[idx].~T();

        if (idx < m_len - 1) {
            if constexpr (dataIsTrivial) {
                core::memcopy(m_data + idx, m_data + idx + 1, m_len - idx - 1);
            }
            else {
                for (size_type i = idx; i < m_len - 1; ++i) {
                    if constexpr (std::is_move_constructible_v<T>) {
                        new (&m_data[i]) T(std::move(m_data[i + 1]));
                    }
                    else {
                        static_assert(std::is_copy_constructible_v<T>);
                        new (&m_data[i]) T(m_data[i + 1]);
                    }
                    m_data[i].~T();
                }
            }
        }

        m_len--;
    }

    value_type pop() {
        Assert(m_len > 0, "pop() called on empty ArrList");
        m_len--;
        value_type ret = std::move(m_data[m_len]);
        m_data[m_len].~T();
        return ret;
    }

    void assign(const value_type& val, size_type from, size_type to) {
        Assert(from <= to, "Invalid range for assign operation.");
        Assert(from <= m_len, "Invalid range for assign operation.");
        Assert(to <= m_len, "Invalid range for assign operation.");

        if (to > from) {
            if constexpr (dataIsTrivial) {
                core::memset(m_data + from, val, to - from);
            }
            else {
                for (size_type i = from; i < to; i++) {
                    new (&m_data[i]) T(val);
                }
            }
        }
    }

    void replaceWith(const value_type& val, size_type count) {
        if (count > m_len) {
            // Very slow path.
            *this = ArrList(count, val);
        }
        else {
            m_len = count;
            assign(val, 0, m_len);
        }
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

    // NOTE: Reason for using assign operator instead of copy constructor -
    //       "placement new expression is not supported in constant expressions before C++2c"
    static_assert(std::is_copy_assignable_v<T> || std::is_move_assignable_v<T>,
                "The provided T does not satisfy the requirements for ArrStatic");

    constexpr ArrStatic() : m_len(0) {}
    constexpr ArrStatic(const ArrStatic& other) = default;
    constexpr ArrStatic(ArrStatic&& other) = default;
    constexpr ArrStatic(size_type len, const T& v) : m_len(len) {
        for (size_type i = 0; i < m_len; i++) {
            if constexpr (std::is_move_constructible_v<T>) {
                m_data[i] = std::move(v);
            }
            else {
                m_data[i] = T(v);
            }
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

    core::Memory<const value_type> memView() const { return { this->data(), this->len() }; }
    core::Memory<value_type>       mem()           { return { this->data(), this->len() }; }

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
            if constexpr (std::is_move_constructible_v<T>) {
                m_data[i + m_len] = std::move(val[i]);
            }
            else {
                static_assert(std::is_copy_constructible_v<T>);
                m_data[i + m_len] = val[i];
            }
        }
        m_len += len;
    }

    constexpr void remove(size_type idx) {
        if (idx < m_len - 1) {
            for (size_type i = idx; i < m_len - 1; ++i) {
                if constexpr (std::is_move_constructible_v<T>) {
                    m_data[i] = std::move(m_data[i + 1]);
                }
                else {
                    static_assert(std::is_copy_constructible_v<T>);
                    m_data[i] = m_data[i + 1];
                }
            }
        }

        m_len--;
    }

    constexpr value_type pop() {
        --m_len;
        return std::move(m_data[m_len]);
    }

    constexpr void assign(const value_type& val, size_type from, size_type to) {
        if (to > from) {
            for (size_type i = from; i < to; i++) {
                if constexpr (std::is_move_constructible_v<T>) {
                    m_data[i] = std::move(val);
                }
                else {
                    m_data[i] = val;
                }
            }
        }
    }

    constexpr void replaceWith(const value_type& val, size_type count) {
        *this = ArrStatic(count, val);
    }

private:
    value_type m_data[N];
    size_type m_len;
};

static_assert(std::is_standard_layout_v<ArrStatic<i32, 3>>, "ArrStatic should be trivial");
static_assert(std::is_standard_layout_v<ArrStatic<core::Memory<char>, 3>>, "ArrStatic should be trivial");

namespace detail {

template<typename TArg, typename ...Args>
constexpr auto _createArrStatic(TArg first, Args... rest) {
    auto ret = ArrStatic<TArg, (sizeof...(rest) + 1)>();
    ret.push(first);
    auto f = [&](auto arg) { ret.push(arg); };
    (f(rest), ...);
    return ret;
}

}

template<typename ...Args>
constexpr auto createArrStatic(Args... args) {
    static_assert(sizeof...(Args) > 0, "createArrStatic requires at least one argument");
    return detail::_createArrStatic(args...);
}

} // namespace core
