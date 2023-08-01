#pragma once

#include <alloc.h>
#include <char_ptr.h>
#include <core_traits.h>
#include <types.h>

namespace core {

// FIXME: Should extend the alloc api with a calloc function, instead of using memset here.

using namespace coretypes;

struct str_view {
    using data_type = char;
    using size_type = ptr_size;

    const data_type* buff;
    size_type len;

    constexpr str_view() : buff(nullptr), len(0) {}
    constexpr str_view(const data_type* buff) : buff(buff), len(core::cptr_len(buff)) {}
    constexpr str_view(const data_type* buff, size_type len) : buff(buff), len(len) {}

    constexpr const data_type* data() const { return buff; }
};

template <typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct str_builder {
    using data_type      = char;
    using size_type      = ptr_size;
    using allocator_type = TAllocator;

    str_builder() : m_data(nullptr), m_cap(0), m_len(0) {}
    str_builder(const str_builder<TAllocator>&) = delete; // prevent copy ctor
    str_builder(str_builder<TAllocator>&& other) : m_data(other.m_data), m_cap(other.m_cap), m_len(other.m_len) {
        if (this == &other) return;
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    str_builder(size_type len) : m_data(nullptr), m_cap(len + 1), m_len(len) {
        allocate_to_cap();
    }
    str_builder(size_type len, size_type cap) : m_data(nullptr), m_cap(cap), m_len(len) {
        Assert(m_cap >= m_len);
        if (m_cap == m_len) m_cap++; // +1 for null terminator
        allocate_to_cap();
    }
    str_builder(const data_type* cptr) {
        m_len = core::cptr_len(cptr);
        m_cap = m_len + 1; // +1 for null terminator
        m_data = reinterpret_cast<data_type*>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
        core::cptr_copy(m_data, cptr, m_len);
    }
    str_builder(str_view view) {
        m_len = view.len;
        m_cap = m_len + 1; // +1 for null terminator
        m_data = reinterpret_cast<data_type*>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
        core::cptr_copy(m_data, view.buff, m_len);
    }
    ~str_builder() { free(); }

    str_builder<TAllocator>& operator=(const str_builder<TAllocator>&) = delete; // prevent copy assignment
    str_builder<TAllocator>& operator=(size_type) = delete; // prevent len assignment
    str_builder<TAllocator>& operator=(str_builder<TAllocator>&& other) {
        if (this == &other) return *this;
        free(); // very important to free before assigning new data.
        m_data = other.m_data;
        m_cap = other.m_cap;
        m_len = other.m_len;
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
        return *this;
    }
    str_builder<TAllocator>& operator=(const data_type* cptr) {
        free();
        m_len = core::cptr_len(cptr);
        m_cap = m_len + 1; // +1 for null terminator
        m_data = reinterpret_cast<data_type*>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
        core::cptr_copy(m_data, cptr, m_len);
        return *this;
    }
    str_builder<TAllocator>& operator=(str_view view) {
        free();
        m_len = view.len;
        m_cap = m_len + 1; // +1 for null terminator
        m_data = reinterpret_cast<data_type*>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
        core::cptr_copy(m_data, view.buff, m_len);
        return *this;
    }

    const char*     allocator_name() const { return core::allocator_name<allocator_type>(); }
    size_type       cap()            const { return m_cap; }
    size_type       byte_cap()       const { return m_cap * sizeof(data_type); }
    size_type       len()            const { return m_len; }
    size_type       byte_len()       const { return m_len * sizeof(data_type); }
    str_view        view()           const { return str_view(m_data, m_len); }
    bool            empty()          const { return m_len == 0; }

    void clear() {
        m_len = 0;
        if (m_data != nullptr) m_data[0] = '\0';
    }

    void free() {
        if (m_data == nullptr) return;
        clear();
        m_cap = 0;
        core::free<allocator_type>(m_data);
        m_data = nullptr;
    }

    str_builder<TAllocator> copy() const {
        str_builder<TAllocator> copy(m_len, m_cap);
        core::memcopy(copy.m_data, m_data, m_len * sizeof(data_type));
        return copy;
    }

    data_type& at(size_type idx)                     { Assert(idx < m_len); return m_data[idx]; }
    const data_type& at(size_type idx)         const { Assert(idx < m_len); return m_data[idx]; }
    data_type& operator[](size_type idx)             { return at(idx); }
    const data_type& operator[](size_type idx) const { return at(idx); }

    data_type& first()             { return at(0); }
    const data_type& first() const { return at(0); }
    data_type& last()              { return at(m_len - 1); }
    const data_type& last()  const { return at(m_len - 1); }

    void take_ownership_from(data_type** ptr) {
        free();
        Assert(ptr != nullptr && *ptr != nullptr);
        m_data = reinterpret_cast<data_type*>(*ptr);
        m_len = core::cptr_len(*ptr);
        m_cap = m_len + 1; // +1 for null terminator
        *ptr = nullptr;
    }

    data_type* steal_ownership() {
        data_type* res = m_data;
        m_data = nullptr;
        m_cap = 0;
        m_len = 0;
        return res;
    }

    str_builder<TAllocator>& append(const data_type& val) {
        if (m_len + 1 >= m_cap) { // +1 for null terminator
            reserve(m_cap == 0 ? 2 : m_cap * 2);
        }
        m_data[m_len] = val;
        m_len++;
        return *this;
    }

    str_builder<TAllocator>& append(const data_type* cptr, size_type len) {
        if (len == 0) return *this;
        if (m_len + len + 1 > m_cap) { // +1 for null terminator
            reserve(m_cap == 0 ? len : m_cap * 2);
        }
        core::cptr_copy(m_data + m_len, cptr, len);
        m_len += len;
        return *this;
    }

    str_builder<TAllocator>& append(const data_type* cptr) {
        return append(cptr, core::cptr_len(cptr));
    }

    str_builder<TAllocator>& append(str_view view) {
        return append(view.buff, view.len);
    }

    str_builder<TAllocator>& fill(const data_type& val) {
        if (m_data != nullptr) core::memset(m_data, val, m_len * sizeof(data_type));
        return *this;
    }

    void reserve(size_type newCap) {
        if (newCap <= m_cap) {
            // shrink
            m_len = m_len > newCap ? newCap - 1 : m_len;
            m_cap = newCap;
            return;
        }

        // reallocate
        data_type* newData = reinterpret_cast<data_type *>(core::alloc<allocator_type>(newCap * sizeof(data_type)));
        Assert(newData != nullptr);
        core::memset(newData, 0, newCap * sizeof(data_type));
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len * sizeof(data_type));
            core::free<allocator_type>(m_data);
        }
        m_data = newData;
        m_cap = newCap;
    }

private:
    data_type* m_data;
    size_type  m_cap;
    size_type  m_len;

    void allocate_to_cap() {
        if (m_cap > 0) {
            m_data = reinterpret_cast<data_type*>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
            Assert(m_data != nullptr);
            core::memset(m_data, 0, m_cap * sizeof(data_type));
        }
    }
};

static_assert(core::is_standard_layout_v<str_builder<>>);

} // namespace core
