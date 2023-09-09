#pragma once

#include <types.h>
#include <alloc.h>
#include <mem.h>
#include <core_traits.h>

namespace core {

using namespace coretypes;

/**
 * Implementation of a hash map using open addressing with linear probing.
 *
 * TODO2: [PERFORMACE] No real consideration has been given to performance.
*/

// FIXME: Create at least as many tests as t-arr.

template <typename T> addr_size hash(const T& key);
template <typename T> bool eq(const T& a, const T& b);

template<typename TKey, typename TValue, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct hash_map {
    constexpr static addr_size DEFAULT_CAP = 2;

    using data_type      = TValue;
    using key_type       = TKey;
    using map_type       = hash_map<TKey, TValue, TAllocator>;
    using size_type      = addr_size;
    using allocator_type = TAllocator;

    struct bucket {
        key_type key;
        data_type data;
        bool occupied;
    };

    static constexpr bool dataIsTrivial = core::is_trivial_v<data_type>;
    static constexpr bool dataHasTrivialDestructor = core::is_trivially_destructible_v<data_type>;

    static_assert(core::is_pod_v<TKey>, "TKey must be pod type");

    hash_map() : m_buckets(nullptr), m_cap(0), m_len(0) {};
    hash_map(size_type cap) : m_buckets(nullptr), m_cap(core::align_to_pow2(cap)), m_len(0) {}
    hash_map(const map_type& other) = delete; // prevent copy ctor
    hash_map(map_type&& other) : m_buckets(other.m_buckets), m_cap(other.m_cap), m_len(other.m_len) {
        if (this == &other) return;
        other.m_buckets = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    ~hash_map() { free(); }

    map_type& operator=(const map_type& other) = delete; // prevent copy assignment
    map_type& operator=(size_type) = delete; // prevent size assignment
    map_type& operator=(map_type&& other) {
        if (this == &other) return *this;
        free(); // very important to free before assigning new data.
        m_buckets = other.m_buckets;
        m_cap = other.m_cap;
        m_len = other.m_len;
        other.m_buckets = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
        return *this;
    }

    const char* allocator_name() const { return core::allocator_name<allocator_type>(); }
    size_type   cap()            const { return m_cap; }
    size_type   byte_cap()       const { return m_cap * sizeof(bucket); }
    size_type   len()            const { return m_len; }
    size_type   byte_len()       const { return m_len * sizeof(bucket); }
    bool        empty()          const { return m_len == 0; }

    void clear() {
        if constexpr (!dataIsTrivial) {
            for (size_type i = 0; i < m_cap; ++i) {
                if (m_buckets[i].occupied) {
                    m_buckets[i].data.~data_type();
                    m_buckets[i].occupied = false;
                    m_buckets[i].key = key_type();
                }
            }
        }
        else {
            core::memset(m_buckets, 0, m_cap * sizeof(bucket));
        }

        m_len = 0;
    }

    void free() {
        if (m_buckets == nullptr) return;
        if constexpr (!dataHasTrivialDestructor) {
            // If the data is not trivially destructible call destructors manually:
            for (size_type i = 0; i < m_cap; ++i) {
                if (m_buckets[i].occupied) {
                    m_buckets[i].data.~data_type();
                }
            }
        }
        m_len = 0;
        m_cap = 0;
        core::free<allocator_type>(m_buckets);
        m_buckets = nullptr;
    }

    map_type copy() {
        map_type cpy(m_cap);
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cpy.put(m_buckets[i].key, m_buckets[i].data);
            }
        }
        return cpy;
    }

    data_type* get(const key_type& key) {
        data_type* data = nullptr;
        auto h = hash(key);
        size_type addr = size_type(h) & (m_cap - 1);
        while (addr < m_cap && m_buckets[addr].occupied) {
            if (eq(m_buckets[addr].key, key)) {
                data = &m_buckets[addr].data;
                break;
            }
            addr = (addr + 1) & (m_cap - 1);
        }

        return data;
    }

    map_type& put(const key_type& key, const data_type& data) {
        if (m_len == m_cap) {
            // FIXME: Resize based on a load factor of 0.80 or somthing like that.
            //        Do not use division when calculating the load factor!
            //        Create some sort of a load factor lookup table for powers of 2.
            resize();
        }

        auto h = hash(key);
        size_type addr = size_type(h) & (m_cap - 1);
        while (m_buckets[addr].occupied) {
            if (eq(m_buckets[addr].key, key)) {
                m_buckets[addr].data = data;
                return *this;
            }
            addr = (addr + 1) & (m_cap - 1);
        }
        m_buckets[addr].key = key;
        m_buckets[addr].data = data;
        m_buckets[addr].occupied = true;
        m_len++;
        return *this;
    }

    template <typename TKeyCb>
    map_type& keys(TKeyCb cb) {
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cb(m_buckets[i].key);
            }
        }
        return *this;
    }

    template <typename TDataCb>
    map_type& values(TDataCb cb) {
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cb(m_buckets[i].data);
            }
        }
        return *this;
    }

private:
    void resize() {
        size_type newCap = m_cap < DEFAULT_CAP ? DEFAULT_CAP : m_cap * 2;
        hash_map newMap;
        newMap.m_buckets = reinterpret_cast<bucket*>(core::alloc<allocator_type>(newCap * sizeof(bucket)));
        core::memset(newMap.m_buckets, 0, newCap * sizeof(bucket));
        newMap.m_cap = newCap;
        newMap.m_len = m_len;

        if (m_buckets) {
            for (size_type i = 0; i < m_cap; ++i) {
                if (m_buckets[i].occupied) {
                    newMap.put(m_buckets[i].key, m_buckets[i].data);
                }
            }
        }

        *this = core::move(newMap);
    }

    bucket* m_buckets;
    size_type m_cap; // The capacity must always be a power of 2.
    size_type m_len;
};

static_assert(core::is_standard_layout_v<hash_map<const char*, i32>>, "hash_map must be standard layout");

} // namespace core
