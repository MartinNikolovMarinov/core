#pragma once

#include <API.h>
#include <types.h>
#include <alloc.h>
#include <expected.h>
#include <mem.h>

namespace core {

using namespace coretypes;

template<typename T, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct CORE_API_EXPORT arr {
    using data_type      = T;
    using size_type      = ptr_size;
    using allocator_type = TAllocator;

    constexpr arr() : m_data(nullptr), m_cap(0), m_len(0) {}
    constexpr arr(size_type len) : m_cap(len), m_len(len) {
        m_data = reinterpret_cast<data_type *>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
    }
    constexpr arr(size_type len, size_type cap) : m_cap(cap), m_len(len) {
        m_data = reinterpret_cast<data_type *>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
        Assert(m_cap >= m_len);
    }
    constexpr arr(const arr& other) : m_cap(other.m_cap), m_len(other.m_len) {
        m_data = reinterpret_cast<data_type *>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
        core::memcopy(m_data, other.m_data, m_len * sizeof(data_type));
    }
    constexpr arr(arr&& other) : m_data(other.m_data), m_cap(other.m_cap), m_len(other.m_len) {
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    ~arr() { free(); }

    // Ensist on move for equals operator. Prevent coping unintentionally using = !
    constexpr arr& operator=(const arr& other) = delete;
    constexpr arr& operator=(arr&& other) {
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

    constexpr const char* allocator_name() const { return core::allocator_name<allocator_type>(); }
    constexpr size_type   cap()            const { return m_cap; }
    constexpr size_type   len()            const { return m_len; }
    constexpr data_type*  data()           const { return m_data; }
    constexpr bool        empty()          const { return m_len == 0; }
    constexpr void        clear()                { m_len = 0; }

    constexpr arr<T, TAllocator> copy() {
        arr<T, TAllocator> result(*this);
        return result;
    }

    constexpr void free() {
        if (m_data == nullptr) return;
        clear();
        m_cap = 0;
        core::free<allocator_type>(m_data);
        m_data = nullptr;
    }

    constexpr data_type& at(size_type idx)                     { Assert(idx <= m_len); return m_data[idx]; }
    constexpr const data_type& at(size_type idx) const         { Assert(idx <= m_len); return m_data[idx]; }
    constexpr data_type& operator[](size_type idx)             { return at(idx); }
    constexpr const data_type& operator[](size_type idx) const { return at(idx); }

    constexpr data_type& first()             { Assert(m_len > 0); return m_data[0]; }
    constexpr const data_type& first() const { Assert(m_len > 0); return m_data[0]; }
    constexpr data_type& last()              { Assert(m_len > 0); return m_data[m_len - 1]; }
    constexpr const data_type& last() const  { Assert(m_len > 0); return m_data[m_len - 1]; }

    constexpr arr& append(const data_type& val) {
        if (m_len == m_cap) {
            resize(m_cap == 0 ? 1 : m_cap * 2);
        }
        copy_data_at(val, m_len++);
        return *this;
    }

    constexpr arr& append(data_type&& val) {
        if (m_len == m_cap) {
            resize(m_cap == 0 ? 1 : m_cap * 2);
        }
        steal_data_at(core::move(val), m_len++);
        return *this;
    }

    constexpr arr& fill(const data_type& val) {
        for (size_type i = 0; i < m_len; ++i) {
            copy_data_at(val, i);
        }
        return *this;
    }

    constexpr arr& fill(data_type&& val) {
        for (size_type i = 0; i < m_len; ++i) {
            steal_data_at(core::move(val), i);
        }
        return *this;
    }

    constexpr void resize(size_type newCap) {
        if (newCap <= m_cap) {
            m_len = m_len > newCap ? newCap : m_len;
            m_cap = newCap;
            return;
        }

        // reallocate
        data_type* newData = reinterpret_cast<data_type *>(core::alloc<allocator_type>(newCap * sizeof(data_type)));
        // NOTE: Choosing not to clear the new memory here might bite me in the ass later.
        Assert(newData != nullptr);
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len * sizeof(data_type));
            core::free<allocator_type>(m_data);
        }
        m_data = newData;
        m_cap = newCap;
    }

private:
    data_type *m_data;
    size_type m_cap;
    size_type m_len;

    // This calls the data_type MOVE constructor onto the rawBytes.
    // This makes sure that raw memory that is actually used will call a constructor.
    inline void steal_data_at(data_type&& val, size_type pos) {
        auto& rawBytes = m_data[pos];
        new (reinterpret_cast<void*>(&rawBytes)) data_type(core::move(val));
    }

    // This calls the data_type COPY constructor onto the rawBytes.
    // This makes sure that raw memory that is actually used will call a constructor.
    inline void copy_data_at(const data_type& val, size_type pos) {
        auto& rawBytes = m_data[pos];
        new (reinterpret_cast<void*>(&rawBytes)) data_type(val);
    }
};

} // namespace core
