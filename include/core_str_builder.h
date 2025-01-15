#pragma once

#include <core_API.h>
#include <core_assert.h>
#include <core_exec_ctx.h>
#include <core_mem.h>
#include <core_str_view.h>
#include <core_traits.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

// TODO2: [PERFORMANCE] Might want to implement small string optimization.

template <AllocatorId TAllocId = DEFAULT_ALLOCATOR_ID>
struct CORE_API_EXPORT StrBuilder {
    inline static core::AllocatorContext& allocator = core::getAllocator(TAllocId);

    using value_type = char;
    using size_type  = addr_size;

    NO_COPY(StrBuilder);

    StrBuilder() : m_data(nullptr), m_cap(0), m_len(0) {}
    StrBuilder(StrBuilder&& other)
        : m_data(other.m_data)
        , m_cap(other.m_cap)
        , m_len(other.m_len) {
            if (this == &other) return;
            other.m_data = nullptr;
            other.m_cap = 0;
            other.m_len = 0;
        }

    StrBuilder(size_type cap) {
        if (cap > 0) {
            m_len = 0;
            m_cap = cap + 1;
            m_data = reinterpret_cast<value_type*>(allocator.zeroAlloc(m_cap, sizeof(value_type)));
        }
        else {
            m_data = nullptr;
            m_cap = 0;
            m_len = 0;
        }
    }
    StrBuilder(size_type len, const value_type& val) {
        m_len = len;
        m_cap = len + 1;
        m_data = reinterpret_cast<value_type*>(allocator.alloc(m_cap, sizeof(value_type)));
        core::memset(m_data, val, m_len);
        m_data[m_len] = '\0';
    }
    StrBuilder(const StrView& view) {
        assignFromCstr(&this->m_data, this->m_len, this->m_cap, view.data(), view.len());
    }
    StrBuilder(StrView&& view) {
        assignFromCstr(&this->m_data, this->m_len, this->m_cap, view.data(), view.len());
        view.ptr = nullptr;
        view.length = 0;
    }

    ~StrBuilder() { free(); }

    StrBuilder& operator=(StrBuilder&& other) {
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
    StrBuilder& operator=(const StrView& view) {
        clear();
        return append(view.data(), view.len());
    }
    StrBuilder& operator=(StrView&& view) {
        clear();
        auto& ret = append(view.data(), view.len());
        view.ptr = nullptr;
        view.length = 0;
        return ret;
    }

    size_type cap()     const { return m_cap; }
    size_type byteCap() const { return m_cap * sizeof(value_type); }
    size_type len()     const { return m_len; }
    size_type byteLen() const { return m_len * sizeof(value_type); }
    bool      empty()   const { return m_len == 0; }

    StrView view() const { return core::sv(m_data, m_len); }

    bool eq(const StrBuilder& other) const {
        return view().eq(other.view());
    }

    bool eq(const StrView& other) const {
        return view().eq(other);
    }

    void clear() { m_len = 0; }

    void free() {
        if (m_data != nullptr) {
            allocator.free(m_data, m_cap, sizeof(value_type));
            m_data = nullptr;
        }
        m_len = 0;
        m_cap = 0;
    }

    value_type& at(size_type idx)                     { Assert(idx < m_len); return m_data[idx]; }
    const value_type& at(size_type idx)         const { Assert(idx < m_len); return m_data[idx]; }
    value_type& operator[](size_type idx)             { return at(idx); }
    const value_type& operator[](size_type idx) const { return at(idx); }

    value_type& first()             { return at(0); }
    const value_type& first() const { return at(0); }
    value_type& last()              { return at(m_len - 1); }
    const value_type& last()  const { return at(m_len - 1); }

    StrBuilder copy() const {
        StrBuilder cpy;
        cpy.m_data = reinterpret_cast<value_type*>(allocator.alloc(m_cap, sizeof(value_type)));
        cpy.m_cap = m_cap;
        cpy.m_len = m_len;
        core::memcopy(cpy.m_data, m_data, cpy.m_cap);
        return cpy;
    }

    void reset(value_type** ptr, addr_size len, addr_size cap) {
        free();

        if (ptr) {
            m_data = *ptr;
            *ptr = nullptr;
        }
        else {
            m_data = nullptr;
        }

        m_len = len;
        m_cap = cap;
    }

    value_type* release(addr_size& len, addr_size& cap) {
        value_type* res = m_data;
        cap = m_cap;
        len = m_len;

        m_data = nullptr;
        m_cap = 0;
        m_len = 0;

        return res;
    }

    StrBuilder& append(const value_type& val) {
        if (m_len + 1 >= m_cap) {
            ensureCap(m_cap == 0 ? 2 : m_cap * 2);
        }
        m_data[m_len] = val;
        m_len++;
        return *this;
    }

    StrBuilder& append(const value_type* cstr, size_type len) {
        if (len == 0) return *this;

        addr_size effectiveLen = m_len + len;
        if (effectiveLen + 1 >= m_cap) {
            ensureCap(effectiveLen + m_cap * 2 + 1);
        }

        core::memcopy(m_data + m_len, cstr, len);
        m_len += len;
        return *this;
    }

    StrBuilder& append(const StrView& view) {
        return append(view.data(), view.len());
    }

    void ensureCap(size_type newCap) {
        if (newCap <= m_cap) {
            return;
        }

        value_type* newData = reinterpret_cast<value_type *>(allocator.zeroAlloc(newCap, sizeof(value_type)));
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len);
            allocator.free(m_data, m_cap, sizeof(value_type));
        }
        m_data = newData;
        m_cap = newCap;
    }

private:
    void assignFromCstr(value_type** value, size_type& len, size_type& cap,
                    const char* cstr, size_type cstrLen) {
        if (cstrLen > 0) {
            len = cstrLen;
            cap = cstrLen + 1;
            *value = reinterpret_cast<value_type*>(allocator.alloc(cap, sizeof(value_type)));
            core::memcopy(*value, cstr, len);
            (*value)[len] = '\0';
        }
        else {
            *value = nullptr;
            len = 0;
            cap = 0;
        }
    }

    value_type* m_data;
    size_type  m_cap;
    size_type  m_len;
};

static_assert(std::is_standard_layout_v<StrBuilder<>>);

} // namespace core
