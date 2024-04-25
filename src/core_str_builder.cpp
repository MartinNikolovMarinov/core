#include <core_str_builder.h>

#include <core_exec_ctx.h>
#include <core_mem.h>

namespace core {

using value_type = StrBuilder::value_type;
using size_type = StrBuilder::size_type;

namespace {

void assignFromCptr(value_type** value, size_type& len, size_type& cap,
                    const char* cptr, size_type cptrLen) {
    if (cptrLen > 0) {
        len = cptrLen;
        cap = cptrLen + 1;
        *value = reinterpret_cast<value_type*>(core::alloc(cap, sizeof(StrBuilder::value_type)));
        core::memcopy(*value, cptr, len * sizeof(StrBuilder::value_type));
        (*value)[len] = '\0';
    }
    else {
        *value = nullptr;
        len = 0;
        cap = 0;
    }
}

} // namespace


StrBuilder::StrBuilder() : m_data(nullptr), m_cap(0), m_len(0) {}
StrBuilder::StrBuilder(StrBuilder&& other)
    : m_data(other.m_data)
    , m_cap(other.m_cap)
    , m_len(other.m_len) {
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }

StrBuilder::StrBuilder(size_type cap) {
    if (cap > 0) {
        m_len = 0;
        m_cap = cap + 1;
        m_data = reinterpret_cast<value_type*>(core::zeroAlloc(m_cap, sizeof(value_type)));
    }
    else {
        m_data = nullptr;
        m_cap = 0;
        m_len = 0;
    }
}
StrBuilder::StrBuilder(size_type len, const value_type& val) {
    m_len = len;
    m_cap = len + 1;
    m_data = reinterpret_cast<value_type*>(core::alloc(m_cap, sizeof(value_type)));
    core::memfill(m_data, m_len, val);
    m_data[m_len] = '\0';
}
StrBuilder::StrBuilder(const StrView& view) {
    assignFromCptr(&this->m_data, this->m_len, this->m_cap, view.data(), view.len());
}
StrBuilder::StrBuilder(StrView&& view) {
    assignFromCptr(&this->m_data, this->m_len, this->m_cap, view.data(), view.len());
    view.ptr = nullptr;
    view.length = 0;
}

StrBuilder::~StrBuilder() { free(); }

StrBuilder& StrBuilder::operator=(StrBuilder&& other) {
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
StrBuilder& StrBuilder::operator=(const StrView& view) {
    clear();
    return append(view.data(), view.len());
}
StrBuilder& StrBuilder::operator=(StrView&& view) {
    clear();
    auto& ret = append(view.data(), view.len());
    view.ptr = nullptr;
    view.length = 0;
    return ret;
}

bool StrBuilder::eq(const StrBuilder& other) const {
    return view().eq(other.view());
}

bool StrBuilder::eq(const StrView& other) const {
    return view().eq(other);
}

void StrBuilder::clear() { m_len = 0; }

void StrBuilder::free() {
    if (m_data != nullptr) {
        core::free(m_data, m_cap, sizeof(value_type));
        m_data = nullptr;
    }
    m_len = 0;
    m_cap = 0;
}

StrBuilder StrBuilder::copy() const {
    StrBuilder cpy;
    cpy.m_data = reinterpret_cast<value_type*>(core::alloc(m_cap, sizeof(value_type)));
    cpy.m_cap = m_cap;
    cpy.m_len = m_len;
    core::memcopy(cpy.m_data, m_data, cpy.m_cap * sizeof(value_type));
    return cpy;
}

void StrBuilder::reset(value_type** ptr, addr_size len, addr_size cap) {
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

value_type* StrBuilder::release(addr_size& len, addr_size& cap) {
    value_type* res = m_data;
    cap = m_cap;
    len = m_len;

    m_data = nullptr;
    m_cap = 0;
    m_len = 0;

    return res;
}

StrBuilder& StrBuilder::append(const value_type& val) {
    if (m_len + 1 >= m_cap) {
        ensureCap(m_cap == 0 ? 2 : m_cap * 2);
    }
    m_data[m_len] = val;
    m_len++;
    return *this;
}

StrBuilder& StrBuilder::append(const value_type* cptr, size_type len) {
    if (len == 0) return *this;

    addr_size effectiveLen = m_len + len;
    if (effectiveLen + 1 >= m_cap) {
        ensureCap(effectiveLen + m_cap * 2 + 1);
    }

    core::cptrCopy(m_data + m_len, cptr, len);
    m_len += len;
    return *this;
}

StrBuilder& StrBuilder::append(const StrView& view) {
    return append(view.data(), view.len());
}

void StrBuilder::ensureCap(size_type newCap) {
    if (newCap <= m_cap) {
        return;
    }

    value_type* newData = reinterpret_cast<value_type *>(core::zeroAlloc(newCap, sizeof(value_type)));
    if (m_data != nullptr) {
        core::memcopy(newData, m_data, m_len * sizeof(value_type));
        core::free(m_data, m_cap, sizeof(value_type));
    }
    m_data = newData;
    m_cap = newCap;
}


} // namespace core
