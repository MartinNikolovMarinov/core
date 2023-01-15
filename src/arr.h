#pragma once

#include <API.h>
#include <types.h>
#include <alloc.h>
#include <expected.h>
#include <mem.h>

// FIXME: Check the asm that the arr code generates. It should be a very cheap abstraction if it's done right.

namespace core {

using namespace coretypes;

// FIXME: Think about the iterator response. It should be some global enum, but are the values reasonable?
enum CORE_API_EXPORT iter_response { ReadBeforeBegin, ReadPastEnd };

template<typename T, typename TAllocator>
struct CORE_API_EXPORT arr {
    using data_type      = T;
    using size_type      = ptr_size;
    using allocator_type = TAllocator;

    constexpr arr() : m_data(nullptr), m_cap(0), m_len(0) {}
    constexpr arr(size_type len) : m_cap(len), m_len(len) {
        m_data = reinterpret_cast<data_type *>(allocator_type::alloc(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
    }
    constexpr arr(size_type cap, size_type len) : m_cap(cap), m_len(len) {
        m_data = reinterpret_cast<data_type *>(allocator_type::alloc(m_cap * sizeof(data_type)));
        Assert(m_data != nullptr);
        Assert(m_cap >= m_len);
    }
    ~arr() { free(); }

    constexpr const char* allocator_name() const { return allocator_type::allocator_name(); }
    constexpr size_type   cap()            const { return m_cap; }
    constexpr size_type   len()            const { return m_len; }
    constexpr data_type*  data()           const { return m_data; }
    constexpr bool        empty()          const { return m_len == 0; }
    constexpr void        clear()                { m_len = 0; }

    constexpr void free() {
        clear();
        m_cap = 0;
        allocator_type::free(m_data);
        m_data = nullptr;
    }

    constexpr data_type& at(size_type idx)         { Assert(idx <= m_len); return m_data[idx]; }
    constexpr data_type& operator[](size_type idx) { return at(idx); }

    constexpr arr& append(const data_type& val) {
        if (m_len == m_cap) {
            resize(m_cap == 0 ? 1 : m_cap * 2);
        }
        m_data[m_len++] = val;
        return *this;
    }

    constexpr arr& fill(const data_type& val) {
        if (val == 0) {
            core::memset(m_data, 0, m_len * sizeof(data_type));
            return *this;
        }
        for (size_type i = 0; i < m_len; ++i) {
            m_data[i] = val;
        }
        return *this;
    }

    constexpr void resize(size_type newCap) {
        if (newCap == 0) {
            free();
            return;
        }
        if (newCap <= m_cap) {
            m_len = m_len > newCap ? newCap : m_len;
            m_cap = newCap;
            return;
        }

        // reallocate
        data_type* newData = reinterpret_cast<data_type *>(allocator_type::alloc(newCap * sizeof(data_type)));
        Assert(newData != nullptr);
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len * sizeof(data_type));
            allocator_type::free(m_data);
        }
        m_data = newData;
        m_cap = newCap;
    }

    // FIXME: the iterator implementation might need a bit more though put into it.
    struct iter {
        size_type idx;
        arr* array;
        core::expected<data_type*, core::iter_response> next() {
            if (idx >= array->len()) return core::unexpected(core::iter_response::ReadPastEnd);
            return &array->m_data[idx++];
        }
        core::expected<data_type*, core::iter_response> prev() {
            if (idx <= 0) return core::unexpected(core::iter_response::ReadBeforeBegin);
            return &array->m_data[idx--];
        }
    };

    constexpr iter begin() { return iter{0, this}; }
    constexpr iter end()   { return iter{m_len, this}; }

private:
    data_type *m_data;
    size_type m_cap;
    size_type m_len;
};

} // namespace core
