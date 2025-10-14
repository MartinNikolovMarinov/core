#include <core_mem.h>
#include <core_types.h>
#include <core_exec_ctx.h>
#include <math/core_math.h>

#include <new>

namespace core {

template <typename T, AllocatorId TAllocId = DEFAULT_ALLOCATOR_ID> struct Stack;
template <typename T, addr_size N>                                 struct StackStatic;

template <typename T, AllocatorId TAllocId>
struct Stack {
    inline static core::AllocatorContext& allocator = core::getAllocator(TAllocId);

    using value_type = T;
    using size_type = addr_size;

    Stack() : m_data(nullptr), m_cap(0), m_len(0) {}
    Stack(size_type cap) : m_data(nullptr), m_cap(cap), m_len(0) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<value_type*>(allocator.alloc(m_cap, sizeof(value_type)));
        }
    }
    ~Stack() { free(); }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    Stack(Stack&& other) noexcept {
        m_data = other.m_data;
        m_cap = other.m_cap;
        m_len = other.m_len;
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    Stack& operator=(Stack&& other) noexcept {
        if (this == &other) return *this;
        free();
        m_data = other.m_data;
        m_cap = other.m_cap;
        m_len = other.m_len;
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
        return *this;
    }

    size_type cap() const { return m_cap; }
    size_type len() const { return m_len; }
    bool empty() const { return m_len == 0; }

    value_type& top() {
        Assert(m_len > 0, "Stack underflow: top() called on empty stack.");
        return m_data[m_len - 1];
    }
    const value_type& top() const {
        Assert(m_len > 0, "Stack underflow: top() called on empty stack.");
        return m_data[m_len - 1];
    }

    void push(const value_type& val) {
        if (m_len == m_cap)
            ensureCap(m_cap == 0 ? 1 : m_cap * 2);
        new (&m_data[m_len]) value_type(val);
        m_len++;
    }

    void push(value_type&& val) {
        if (m_len == m_cap)
            ensureCap(m_cap == 0 ? 1 : m_cap * 2);
        new (&m_data[m_len]) value_type(std::move(val));
        m_len++;
    }

    value_type&& pop() {
        Assert(m_len > 0, "Stack underflow: pop() called on empty stack.");
        m_len--;
        return std::move(m_data[m_len]);
    }

    void clear() {
        for (size_type i = 0; i < m_len; ++i)
            m_data[i].~T();
        m_len = 0;
    }

    void ensureCap(size_type newCap) {
        if (newCap <= m_cap) {
            return;
        }

        // reallocate
        value_type* newData = reinterpret_cast<value_type *>(allocator.alloc(newCap, sizeof(value_type)));
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len);
            allocator.free(m_data, m_cap, sizeof(value_type));
        }

        m_data = newData;
        m_cap = newCap;
    }

    void free() {
        if (!m_data) return;
        clear();
        allocator.free(m_data, m_cap, sizeof(value_type));
        m_data = nullptr;
        m_cap = 0;
        m_len = 0;
    }

private:
    value_type* m_data;
    size_type m_cap;
    size_type m_len;
};

template <typename T, addr_size N>
struct StackStatic {
    using value_type = T;
    using size_type = addr_size;

    static_assert(std::is_standard_layout_v<T>, "StackStatic should be standard layout");
    static_assert(std::is_trivially_copy_assignable_v<T>, "StackStatic should be trivially copy assignable");
    static_assert(std::is_trivially_move_assignable_v<T>, "StackStatic should be trivially move assignable");

    constexpr StackStatic() : m_len(0) {}
    constexpr StackStatic(const StackStatic& other) = default;
    constexpr StackStatic(StackStatic&& other) = default;
    constexpr StackStatic(size_type len, const T& v) : m_len(len) {
        for (size_type i = 0; i < m_len; i++) {
            m_data[i] = v;
        }
    }

    constexpr StackStatic& operator=(const StackStatic& other) = default;
    constexpr StackStatic& operator=(StackStatic&& other) = default;

    constexpr static size_type  cap()           { return N; }
    constexpr static size_type  byteCap()       { return N * sizeof(value_type); }
    constexpr size_type         len()     const { return m_len; }
    constexpr size_type         byteLen() const { return m_len * sizeof(value_type); }
    constexpr bool              empty()   const { return m_len == 0; }

    constexpr value_type*       data()          { return m_data; }
    constexpr const value_type* data()    const { return m_data; }

    constexpr value_type&       at(size_type idx)               { return m_data[idx]; }
    constexpr const value_type& at(size_type idx)         const { return m_data[idx]; }
    constexpr value_type&       operator[](size_type idx)       { return at(idx); }
    constexpr const value_type& operator[](size_type idx) const { return at(idx); }

    constexpr value_type& top() {
        Assert(m_len > 0, "Stack underflow: top() on empty stack.");
        return m_data[m_len - 1];
    }

    constexpr const value_type& top() const {
        Assert(m_len > 0, "Stack underflow: top() on empty stack.");
        return m_data[m_len - 1];
    }

    constexpr void push(const value_type& val) {
        Assert(m_len < N, "Stack overflow in push().");
        m_data[m_len++] = val;
    }

    constexpr void push(value_type&& val) {
        Assert(m_len < N, "Stack overflow in push().");
        m_data[m_len++] = std::move(val);
    }

    constexpr value_type&& pop() {
        Assert(m_len > 0, "Stack underflow in pop().");
        --m_len;
        return std::move(m_data[m_len]);
    }

    constexpr void clear() { m_len = 0; }

    constexpr void assign(const value_type& val, size_type from, size_type to) {
        Assert(from <= to && to <= m_len, "Invalid assign range.");
        for (size_type i = from; i < to; i++) {
            m_data[i] = val;
        }
    }

private:
    value_type m_data[N];
    size_type  m_len;
};

static_assert(std::is_standard_layout_v<StackStatic<i32, 4>>, "StackStatic should be standard layout");

} // namespace core
