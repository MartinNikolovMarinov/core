#pragma once

#include <API.h>
#include <types.h>
#include <alloc.h>
#include <mem.h>
#include <core_traits.h>

namespace core {

using namespace coretypes;

template<typename T, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct CORE_API_EXPORT arr {
    using data_type      = T;
    using size_type      = ptr_size;
    using allocator_type = TAllocator;

    static constexpr bool dataIsTrivial = core::is_trivial_v<data_type>;
    static constexpr bool dataHasTrivialDestructor = core::is_trivially_destructable_v<data_type>;

    arr() : m_data(nullptr), m_cap(0), m_len(0) {}
    arr(size_type len) : m_data(nullptr), m_cap(len), m_len(len) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<data_type *>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
            Assert(m_data != nullptr);
            callDefaultCtorsIfTypeIsNonTrivial();
        }
    }
    arr(size_type len, size_type cap) : m_data(nullptr), m_cap(cap), m_len(len) {
        Assert(m_cap >= m_len);
        if (m_cap > 0) {
            m_data = reinterpret_cast<data_type *>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
            Assert(m_data != nullptr);
            callDefaultCtorsIfTypeIsNonTrivial();
        }
    }
    arr(const arr& other) : m_data(nullptr), m_cap(other.m_cap), m_len(other.m_len) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<data_type *>(core::alloc<allocator_type>(m_cap * sizeof(data_type)));
            Assert(m_data != nullptr);
            copyDataAt(other.m_data, 0, m_len);
        }
    }
    arr(arr&& other) : m_data(other.m_data), m_cap(other.m_cap), m_len(other.m_len) {
        if (this == &other) return;
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    ~arr() { free(); }

    // Prevent copy assignment, because it is unwanted behaviour most of the time.
    //This does not prevent auto arr2 = arr1; but prevents arr2 = arr1; !
    arr& operator=(const arr& other) = delete;
    arr& operator=(arr&& other) {
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

    const char* allocator_name() const { return core::allocator_name<allocator_type>(); }
    size_type   cap()            const { return m_cap; }
    size_type   byte_cap()       const { return m_cap * sizeof(data_type); }
    size_type   len()            const { return m_len; }
    size_type   byte_len()       const { return m_len * sizeof(data_type); }
    data_type*  data()           const { return m_data; }
    bool        empty()          const { return m_len == 0; }
    void        clear()                { m_len = 0; } // FIXME: If the type is not trivially destructible, this will leak memory!

    arr<T, TAllocator> copy() {
        arr<T, TAllocator> result(*this);
        return result;
    }

    void free() {
        if (m_data == nullptr) return;
        if constexpr (!dataHasTrivialDestructor) {
            // For elements that are not trivially destructible call destructors manually:
            for (size_type i = 0; i < m_len; ++i) {
                m_data[i].~T();
            }
        }
        clear();
        m_cap = 0;
        core::free<allocator_type>(m_data);
        m_data = nullptr;
    }

    data_type& at(size_type idx)                     { Assert(idx <= m_len); return m_data[idx]; }
    const data_type& at(size_type idx)         const { Assert(idx <= m_len); return m_data[idx]; }
    data_type& operator[](size_type idx)             { return at(idx); }
    const data_type& operator[](size_type idx) const { return at(idx); }

    data_type& first()             { return at(0); }
    const data_type& first() const { return at(0); }
    data_type& last()              { return at(m_len - 1); }
    const data_type& last()  const { return at(m_len - 1); }

    arr& append(const data_type& val) {
        if (m_len == m_cap) {
            reserve(m_cap == 0 ? 1 : m_cap * 2);
        }
        copyDataAt(val, m_len);
        m_len++;
        return *this;
    }

    arr& append(data_type&& val) {
        if (m_len == m_cap) {
            reserve(m_cap == 0 ? 1 : m_cap * 2);
        }
        stealDataAt(core::move(val), m_len);
        m_len++;
        return *this;
    }

    arr& append(const data_type* val, size_type len) {
        if (m_len + len > m_cap) {
            reserve(m_cap == 0 ? len : m_cap * 2);
        }
        copyDataAt(val, m_len, len);
        m_len += len;
        return *this;
    }

    arr& fill(const data_type& val) {
        for (size_type i = 0; i < m_len; ++i) {
            copyDataAt(val, i);
        }
        return *this;
    }

    void reserve(size_type newCap) {
        if (newCap <= m_cap) {
            if constexpr (dataIsTrivial) {
                m_len = m_len > newCap ? newCap : m_len;
                m_cap = newCap;
            }
            else {
                // For elements that are not trivially destructible call destructors manually:
                for (size_type i = newCap; i < m_len; ++i) {
                    m_data[i].~T();
                }
                m_len = m_len > newCap ? newCap : m_len;
                m_cap = newCap;
            }
            return;
        }

        // reallocate
        data_type* newData = reinterpret_cast<data_type *>(core::alloc<allocator_type>(newCap * sizeof(data_type)));
        // NOTE: Choosing not to clear the new memory here might bite my ass later.
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

    inline void stealDataAt(data_type&& rval, size_type pos) {
        auto& rawBytes = m_data[pos];
        new (reinterpret_cast<void*>(&rawBytes)) data_type(core::move(rval));
    }

    inline void copyDataAt(const data_type& lval, size_type pos) {
        if constexpr (dataIsTrivial) {
            core::memcopy(m_data + pos, &lval, sizeof(data_type));
        }
        else {
            auto& rawBytes = m_data[pos];
            new (reinterpret_cast<void*>(&rawBytes)) data_type(lval);
        }
    }

    inline void copyDataAt(const data_type* pval, size_type pos, size_type len) {
        if constexpr (dataIsTrivial) {
            core::memcopy(m_data + pos, pval, len * sizeof(data_type));
        }
        else {
            for (size_type i = 0; i < len; i++) {
                auto& rawBytes = m_data[i + pos];
                new (reinterpret_cast<void*>(&rawBytes)) data_type(pval[i]);
            }
        }
    }

    inline void callDefaultCtorsIfTypeIsNonTrivial() {
        if constexpr (!dataIsTrivial) {
            for (size_type i = 0; i < m_len; ++i) {
                new (&m_data[i]) data_type();
            }
        }
    }
};

} // namespace core
