#pragma once

#include <core_API.h>
#include <core_assert.h>
#include <core_str_view.h>
#include <core_traits.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

// TODO2: [PERFORMANCE] Might want to implement small string optimization.

struct CORE_API_EXPORT StrBuilder;

struct CORE_API_EXPORT StrBuilder {
    using value_type = char;
    using size_type  = addr_size;

    NO_COPY(StrBuilder);

    StrBuilder();
    StrBuilder(StrBuilder&& other);

    explicit StrBuilder(size_type cap);
    StrBuilder(size_type len, const value_type& val);
    explicit StrBuilder(const StrView& view);
    explicit StrBuilder(StrView&& view);

    ~StrBuilder();

    StrBuilder& operator=(size_type) = delete; // prevent len assignment
    StrBuilder& operator=(StrBuilder&& other);
    StrBuilder& operator=(const StrView& view);
    StrBuilder& operator=(StrView&& view);

    size_type cap()     const { return m_cap; }
    size_type byteCap() const { return m_cap * sizeof(value_type); }
    size_type len()     const { return m_len; }
    size_type byteLen() const { return m_len * sizeof(value_type); }
    bool      empty()   const { return m_len == 0; }

    StrView view() const { return core::sv(m_data, m_len); }

    bool eq(const StrBuilder& other) const;
    bool eq(const StrView& other) const;

    void clear();
    void free();

    StrBuilder copy() const;

    value_type& at(size_type idx)                     { Assert(idx < m_len); return m_data[idx]; }
    const value_type& at(size_type idx)         const { Assert(idx < m_len); return m_data[idx]; }
    value_type& operator[](size_type idx)             { return at(idx); }
    const value_type& operator[](size_type idx) const { return at(idx); }

    value_type& first()             { return at(0); }
    const value_type& first() const { return at(0); }
    value_type& last()              { return at(m_len - 1); }
    const value_type& last()  const { return at(m_len - 1); }

    void reset(value_type** ptr, addr_size len, addr_size cap);
    value_type* release(addr_size& len, addr_size& cap);

    StrBuilder& append(const value_type& val);
    StrBuilder& append(const value_type* cstr, size_type len);
    StrBuilder& append(const StrView& view);

    void ensureCap(size_type newCap);

private:
    value_type* m_data;
    size_type  m_cap;
    size_type  m_len;
};

static_assert(std::is_standard_layout_v<StrBuilder>);

} // namespace core
