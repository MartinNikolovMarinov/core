#pragma once

#include <core_alloc.h>
#include <core_mem.h>
#include <core_traits.h>
#include <core_types.h>
#include <core_exec_ctx.h>
#include <math/core_math.h>

namespace core {

using namespace coretypes;

template<typename T>
struct Arr {
    using value_type = T;
    using size_type = addr_size;

    static constexpr bool dataIsTrivial = std::is_trivial_v<value_type>;
    static constexpr bool dataHasDestructor = std::is_destructible_v<value_type>;

    Arr() : m_data(nullptr), m_cap(0), m_len(0) {}
    Arr(size_type cap) : m_data(nullptr), m_cap(cap), m_len(0) {
        if (m_cap > 0) {
            m_data = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
            Assert(m_data != nullptr);
        }
    }
    Arr(size_type len, const T& val) : m_data(nullptr), m_cap(len), m_len(len) {
        Assert(m_cap >= m_len);
        if (m_cap > 0) {
            m_data = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
            Assert(m_data != nullptr);

            for (size_type i = 0; i < m_len; i++) {
                auto& rawBytes = m_data[i];
                new (reinterpret_cast<void*>(&rawBytes)) value_type(val);
            }
        }
    }
    Arr(const Arr& other) = delete; // prevent copy ctor
    Arr(Arr&& other) : m_data(other.m_data), m_cap(other.m_cap), m_len(other.m_len) {
        if (this == &other) return;
        free();
        other.m_data = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    ~Arr() { free(); }

    Arr& operator=(const Arr& other) = delete; // prevent copy assignment
    Arr& operator=(size_type) = delete; // prevent size assignment
    Arr& operator=(Arr&& other) {
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

    size_type         cap()     const { return m_cap; }
    size_type         byteCap() const { return m_cap * sizeof(value_type); }
    size_type         len()     const { return m_len; }
    size_type         byteLen() const { return m_len * sizeof(value_type); }
    value_type*       data()          { return m_data; }
    const value_type* data()    const { return m_data; }
    bool              empty()   const { return m_len == 0; }

    void clear() {
        if constexpr (!dataHasDestructor) {
            // For elements that are not trivially destructible call destructors manually:
            for (size_type i = 0; i < m_len; ++i) {
                m_data[i].~T();
            }
        }
        m_len = 0;
    }

    Arr copy() {
        value_type* dataCopy = nullptr;
        if (m_cap > 0) {
            dataCopy = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
            Assert(dataCopy != nullptr);
            if constexpr (dataIsTrivial) {
                core::memcopy(dataCopy, m_data, m_len * sizeof(value_type));
            }
            else {
                for (size_type i = 0; i < m_len; i++) {
                    copyDataAt(m_data[i], i);
                }
            }
        }
        Arr result;
        result.m_data = dataCopy;
        result.m_cap = m_cap;
        result.m_len = m_len;
        return result;
    }

    void free() {
        if (m_data == nullptr) return;
        clear();
        core::free(m_data, m_cap * sizeof(value_type));
        m_cap = 0;
        m_data = nullptr;
    }

    value_type& at(size_type idx)                     { Assert(idx < m_len); return m_data[idx]; }
    const value_type& at(size_type idx)         const { Assert(idx < m_len); return m_data[idx]; }
    value_type& operator[](size_type idx)             { return at(idx); }
    const value_type& operator[](size_type idx) const { return at(idx); }

    value_type& first()             { return at(0); }
    const value_type& first() const { return at(0); }
    value_type& last()              { return at(m_len - 1); }
    const value_type& last()  const { return at(m_len - 1); }

    Arr& append(const value_type& val) {
        if (m_len == m_cap) {
            ensureCap(m_cap == 0 ? 1 : m_cap * 2);
        }
        copyDataAt(val, m_len);
        m_len++;
        return *this;
    }

    Arr& append(value_type&& val) {
        if (m_len == m_cap) {
            ensureCap(m_cap == 0 ? 1 : m_cap * 2);
        }
        stealDataAt(std::move(val), m_len);
        m_len++;
        return *this;
    }

    Arr& append(const value_type* val, size_type len) {
        if (m_len + len > m_cap) {
            ensureCap(m_cap <= len ? (m_cap*2 + len) : m_cap * 2);
        }
        copyDataAt(val, m_len, len);
        m_len += len;
        return *this;
    }

    Arr& append(const Arr& other) {
        return append(other.m_data, other.m_len);
    }

    Arr& fill(const value_type& val, addr_size from, addr_size to) {
        if (from >= to) return *this;

        [[maybe_unused]] addr_size overwriteIdx = core::core_min(to, m_len);
        if (to > m_len) {
            ensureCap(to);
            m_len = to;
        }

        addr_size count = to - from;

        if constexpr (dataIsTrivial) {
            core::memfill(m_data + from, count, val);
        }
        else {
            if constexpr (!dataHasDestructor) {
                // Destroy only the elements that are being overwritten:
                for (size_type i = from; i < overwriteIdx; ++i) {
                    m_data[i].~T();
                }
            }

            for (size_type i = from; i < from + count; ++i) {
                copyDataAt(val, i);
            }
        }

        return *this;
    }

    Arr& remove(size_type idx) {
        Assert(idx < m_len);
        if constexpr (!dataHasDestructor) {
            // For elements that are not trivially destructible, call destructors manually:
            m_data[idx].~T();
        }
        if (idx < m_len - 1) {
            for (size_type i = idx; i < m_len - 1; ++i) {
                m_data[i] = m_data[i + 1];
            }
        }
        m_len--;
        return *this;
    }

    // This is the "I know what I am doing" method. Gives ownership of data to the array. The array's destructor will
    // free it, so if a different allocator was used to allocate the data the results are undefined.
    void reset(value_type* data, size_type len) {
        free();
        m_data = data;
        m_cap = len;
        m_len = len;
    }

    void ensureCap(size_type newCap) {
        if (newCap <= m_cap) {
            return;
        }

        // reallocate
        value_type* newData = reinterpret_cast<value_type *>(core::alloc(newCap, sizeof(value_type)));
        // NOTE: Choosing not to clear the new memory here might bite me on the ass later.
        Assert(newData != nullptr);
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len * sizeof(value_type));
            core::free(m_data, m_cap * sizeof(value_type));
        }
        m_data = newData;
        m_cap = newCap;
    }

    value_type* steal(size_type& len, size_type& cap) {
        if constexpr (!dataIsTrivial) {
            Assert(false, "Stealing the raw pointer from an array of non trivial types is strongly discouraged.");
        }

        value_type* res = m_data;
        cap = m_cap;
        len = m_len;
        m_data = nullptr;
        m_cap = 0;
        m_len = 0;
        return res;
    }

private:
    value_type *m_data;
    size_type m_cap;
    size_type m_len;

    inline void stealDataAt(value_type&& rval, size_type pos) {
        auto& rawBytes = m_data[pos];
        new (reinterpret_cast<void*>(&rawBytes)) value_type(std::move(rval));
    }

    inline void copyDataAt(const value_type& lval, size_type pos) {
        if constexpr (dataIsTrivial) {
            m_data[pos] = lval;
        }
        else {
            auto& rawBytes = m_data[pos];
            new (reinterpret_cast<void*>(&rawBytes)) value_type(lval);
        }
    }

    inline void copyDataAt(const value_type* pval, size_type pos, size_type len) {
        if constexpr (dataIsTrivial) {
            core::memcopy(m_data + pos, pval, len * sizeof(value_type));
        }
        else {
            for (size_type i = 0; i < len; i++) {
                auto& rawBytes = m_data[i + pos];
                new (reinterpret_cast<void*>(&rawBytes)) value_type(pval[i]);
            }
        }
    }
};

static_assert(std::is_standard_layout_v<Arr<i32>>, "Arr<i32> must be standard layout");

} // namespace core
