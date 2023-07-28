#pragma once

#include <alloc.h>
#include <arr.h>
#include <char_ptr.h>
#include <core_traits.h>
#include <types.h>

namespace core {

// FIXME: Don't use arr and keep all data null-terminated.

using namespace coretypes;

struct str_view_tmp {
    const char* buff; // Memory might not be null terminated.
    ptr_size len;
};

template <typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct str_builder {
    using data_type      = char;
    using size_type      = ptr_size;
    using allocator_type = TAllocator;

    str_builder() = default;
    str_builder(size_type len) : m_buff(len) { m_buff.fill(0); }
    str_builder(size_type len, size_type cap) : m_buff(len, cap) { m_buff.fill(0); }
    str_builder(const str_builder& other) = delete; // prevent copy ctor
    str_builder(str_builder&& other) : m_buff(core::move(other.m_buff)) {}
    str_builder(const data_type* cptr) { copy_cptr_into_buff(cptr); }
    str_builder(const data_type* cptr, size_type n) { copy_cptr_into_buff(cptr, n); }
    ~str_builder() { m_buff.free(); }

    str_builder& operator=(const str_builder& other) = delete; // Prevent copy assignment
    str_builder& operator=(str_builder&& other) = default;
    str_builder& operator=(const data_type* cptr) { this->copy_cptr_into_buff(cptr); return *this; }

    str_builder copy() const {
        str_builder s;
        s.m_buff = m_buff.copy();
        return s;
    }

    /**
     * \brief Call to get a copy of the null-terminated character string.
     *
     * \param out The buffer to copy the string into. Must be at least len() + 1 bytes long.
    */
    void copy_to(char* out) {
        core::cptr_copy(m_buff.data(), m_buff.len(), out);
        out[m_buff.len()] = '\0';
    }

    const char*  allocator_name() const { return core::allocator_name<allocator_type>(); }
    size_type    cap()            const { return m_buff.cap(); }
    size_type    byte_cap()       const { return m_buff.byte_cap(); }
    size_type    len()            const { return m_buff.len(); }
    size_type    byte_len()       const { return m_buff.byte_len(); }
    str_view_tmp view()           const { return { m_buff.data(), m_buff.len() }; }
    bool         empty()          const { return m_buff.empty(); }

    data_type& at(size_type idx)                     { return m_buff.at(idx); }
    const data_type& at(size_type idx)         const { return m_buff.at(idx); }
    data_type& operator[](size_type idx)             { return m_buff[idx]; }
    const data_type& operator[](size_type idx) const { return m_buff[idx]; }

    data_type& first()             { return m_buff.first(); }
    const data_type& first() const { return m_buff.first(); }
    data_type& last()              { return m_buff.last(); }
    const data_type& last()  const { return m_buff.last(); }

    str_builder& append(data_type val) { m_buff.append(val); return *this; }
    str_builder& append(const data_type* cptr) { this->copy_cptr_into_buff(cptr); return *this; }

    str_builder& fill(data_type val) { m_buff.fill(val); return *this; }

    void reserve(size_type newCap) { m_buff.reserve(newCap); }

private:
    core::arr<data_type, allocator_type> m_buff;

    void copy_cptr_into_buff(const data_type* cptr) {
        copy_cptr_into_buff(cptr, core::cptr_len(cptr));
    }

    void copy_cptr_into_buff(const data_type* cptr, i64 cptrLen) {
        ptr_size newLen = m_buff.len() + cptrLen;
        m_buff.append(cptr, newLen);
    }
};

static_assert(core::is_standard_layout_v<str_builder<>>);

} // namespace core
