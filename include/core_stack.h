#pragma once

#include <core_arr.h>
#include <core_exec_ctx.h>
#include <core_traits.h>
#include <core_types.h>

#include <math/core_math.h>

namespace core {

template <typename T, AllocatorId TAllocId = DEFAULT_ALLOCATOR_ID> struct Stack;
template <typename T, addr_size N>                                 struct StackStatic;

template <typename T, AllocatorId TAllocId>
struct Stack {
    using value_type = T;
    using size_type = addr_size;

    Stack() = default;
    explicit Stack(size_type cap) { m_arr.ensureCap(cap); }
    ~Stack() = default;

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    Stack(Stack&&) noexcept = default;
    Stack& operator=(Stack&&) noexcept = default;

    size_type         cap()     const { return m_arr.cap(); }
    size_type         byteCap() const { return m_arr.byteCap(); }
    size_type         len()     const { return m_arr.len(); }
    size_type         byteLen() const { return m_arr.byteLen(); }
    value_type*       data()          { return m_arr.data(); }
    const value_type* data()    const { return m_arr.data(); }
    bool              empty()   const { return m_arr.empty(); }
    void              clear()         { m_arr.clear(); }

    value_type& top() {
        Assert(m_arr.len() > 0, "Stack underflow: top() called on empty stack.");
        return m_arr[m_arr.len() - 1];
    }
    const value_type& top() const {
        Assert(m_arr.len() > 0, "Stack underflow: top() called on empty stack.");
        return m_arr[m_arr.len() - 1];
    }

    void push(const value_type& val) { m_arr.push(val); }
    void push(value_type&& val) { m_arr.push(std::move(val)); }

    value_type pop() { return m_arr.pop(); }

    void ensureCap(size_type newCap) { m_arr.ensureCap(newCap); }
    void free() { m_arr.free(); }

private:
    ArrList<T, TAllocId> m_arr;
};

template <typename T, addr_size N>
struct StackStatic {
    using value_type = T;
    using size_type = addr_size;

    static_assert(std::is_standard_layout_v<T>, "StackStatic should be standard layout");
    static_assert(std::is_trivially_copy_assignable_v<T>, "StackStatic should be trivially copy assignable");
    static_assert(std::is_trivially_move_assignable_v<T>, "StackStatic should be trivially move assignable");

    constexpr StackStatic() = default;
    constexpr StackStatic(const StackStatic& other) = default;
    constexpr StackStatic(StackStatic&& other) = default;
    constexpr StackStatic(size_type len, const T& v) : m_arr(len, v) {}

    constexpr StackStatic& operator=(const StackStatic& other) = default;
    constexpr StackStatic& operator=(StackStatic&& other) = default;

    constexpr static size_type  cap()           { return ArrStatic<T, N>::cap(); }
    constexpr static size_type  byteCap()       { return ArrStatic<T, N>::byteCap(); }
    constexpr size_type         len()     const { return m_arr.len(); }
    constexpr size_type         byteLen() const { return m_arr.byteLen(); }
    constexpr value_type*       data()          { return m_arr.data(); }
    constexpr const value_type* data()    const { return m_arr.data(); }
    constexpr bool              empty()   const { return m_arr.empty(); }
    constexpr void              clear()         { m_arr.clear(); }

    constexpr value_type& top() {
        Assert(m_arr.len() > 0, "Stack underflow: top() on empty stack.");
        return m_arr[m_arr.len() - 1];
    }

    constexpr const value_type& top() const {
        Assert(m_arr.len() > 0, "Stack underflow: top() on empty stack.");
        return m_arr[m_arr.len() - 1];
    }

    constexpr void push(const value_type& val) {
        Assert(m_arr.len() < N, "Stack overflow in push().");
        m_arr.push(val);
    }

    constexpr void push(value_type&& val) {
        Assert(m_arr.len() < N, "Stack overflow in push().");
        m_arr.push(std::move(val));
    }

    constexpr value_type pop() {
        Assert(m_arr.len() > 0, "Stack underflow in pop().");
        return m_arr.pop();
    }

    constexpr void assign(const value_type& val, size_type from, size_type to) {
        Assert(from <= to && to <= m_arr.len(), "Invalid assign range.");
        m_arr.assign(val, from, to);
    }

private:
    ArrStatic<T, N> m_arr;
};

static_assert(std::is_standard_layout_v<StackStatic<i32, 4>>, "StackStatic should be standard layout");

} // namespace core
