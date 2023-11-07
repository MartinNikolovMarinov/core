#pragma once

#include <types.h>
#include <alloc.h>
#include <mem.h>
#include <core_traits.h>
#include <core_math.h>

namespace core {

using namespace coretypes;

namespace detail {

// The capacity of the hash_map is always a power of 2 to avoid using division in any of the hot code paths.
// This allows the use of a lookup table to determine when to resize the hash_map.
constexpr inline addr_size lookup_max_load_factor(addr_size cap) {
    switch (cap) {
        case 0: [[fallthrough]];
        case 1: [[fallthrough]];
        case 2: [[fallthrough]];
        case 4: [[fallthrough]];
        case 8: [[fallthrough]];
        case 16: return cap; // Smaller maps ^ use a factor of 1.0 to spare memory.
        case 32: return addr_size(32 * 0.80);
        case 64: return addr_size(64 * 0.80);
        case 128: return addr_size(128 * 0.80);
        case 256: return addr_size(256 * 0.80);
        case 512: return addr_size(512 * 0.80);
        case 1024: return addr_size(1024 * 0.80);
        case 2048: return addr_size(2048 * 0.80);
        case 4096: return addr_size(4096 * 0.80);
        case 8192: return addr_size(8192 * 0.80);
        case 16384: return addr_size(16384 * 0.80);
        case 32768: return addr_size(32768 * 0.80);
        case 65536: return addr_size(65536 * 0.80);
        case 131072: return addr_size(131072 * 0.80);
        case 262144: return addr_size(262144 * 0.80);
        case 524288: return addr_size(524288 * 0.80);
        case 1048576: return addr_size(1048576 * 0.80);
        case 2097152: return addr_size(2097152 * 0.80);
        case 4194304: return addr_size(4194304 * 0.80);
        case 8388608: return addr_size(8388608 * 0.80);
        case 16777216: return addr_size(16777216 * 0.80);
        case 33554432: return addr_size(33554432 * 0.80);
        case 67108864: return addr_size(67108864 * 0.80);
        case 134217728: return addr_size(134217728 * 0.80);
        case 268435456: return addr_size(268435456 * 0.80);
        case 536870912: return addr_size(536870912 * 0.80);
        case 1073741824: return addr_size(1073741824 * 0.80);
        case 2147483648: return addr_size(2147483648 * 0.80);
        case 4294967296: return addr_size(4294967296 * 0.80);
        case 8589934592: return addr_size(8589934592 * 0.80);
        case 17179869184: return addr_size(17179869184 * 0.80);
        case 34359738368: return addr_size(34359738368 * 0.80);
        case 68719476736: return addr_size(68719476736 * 0.80);
        case 137438953472: return addr_size(137438953472 * 0.80);
        case 274877906944: return addr_size(274877906944 * 0.80);
        case 549755813888: return addr_size(549755813888 * 0.80);
        case 1099511627776: return addr_size(1099511627776 * 0.80);
        case 2199023255552: return addr_size(2199023255552 * 0.80);
        case 4398046511104: return addr_size(4398046511104 * 0.80);
        case 8796093022208: return addr_size(8796093022208 * 0.80);
        case 17592186044416: return addr_size(17592186044416 * 0.80);
        case 35184372088832: return addr_size(35184372088832 * 0.80);
        case 70368744177664: return addr_size(70368744177664 * 0.80);
        case 140737488355328: return addr_size(140737488355328 * 0.80);
        case 281474976710656: return addr_size(281474976710656 * 0.80);
        case 562949953421312: return addr_size(562949953421312 * 0.80);
        case 1125899906842624: return addr_size(1125899906842624 * 0.80);
        case 2251799813685248: return addr_size(2251799813685248 * 0.80);
        case 4503599627370496: return addr_size(4503599627370496 * 0.80);
        case 9007199254740992: return addr_size(9007199254740992 * 0.80);
    }

    // slow path:
    Assert(false, "lookup_80percent: value is not a power of 2");
    return addr_size(f64(cap) * 0.80);
}

};

/**
 * Implementation of a hash map using open addressing with linear probing.
 *
 * TODO2: [PERFORMACE] No real consideration has been given to performance.
*/

template <typename T> addr_size hash(const T& key);
template <typename T> bool eq(const T& a, const T& b);

template<typename TKey, typename TValue, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct hash_map {
    constexpr static addr_size DEFAULT_CAP = 16;

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
    hash_map(size_type cap) : m_buckets(nullptr), m_cap(core::align_to_pow2(cap)), m_len(0) {
        m_buckets = reinterpret_cast<bucket*>(core::calloc<allocator_type>(m_cap, sizeof(bucket)));
    }
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

    const data_type* get(const key_type& key) const {
        const bucket* b = find_bucket(key);
        return b ? &b->data : nullptr;
    }

    data_type* get(const key_type& key) {
        return const_cast<data_type*>(static_cast<const map_type &>(*this).get(key));
    }

    map_type& put(const key_type& key, const data_type& data) {
        return put_impl(key, data);
    }

    map_type& put(const key_type& key, data_type&& data) {
        return put_impl(key, core::move(data));
    }

    map_type& remove(const key_type& key) {
        bucket* b = find_bucket(key);
        if (b) {
            b->occupied = false;
            b->key = key_type(); // rest key
            b->data = data_type(); // reset data
            m_len--;
        }
        return *this;
    }

    template <typename TKeyCb>
    const map_type& keys(const TKeyCb& cb) const {
        if (m_buckets == nullptr) return *this;
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cb(m_buckets[i].key);
            }
        }
        return *this;
    }

    template <typename TKeyCb>
    map_type& keys(const TKeyCb& cb) {
        return const_cast<map_type&>(static_cast<const map_type&>(*this).keys(cb));
    }

    template <typename TDataCb>
    const map_type& values(const TDataCb& cb) const {
        if (m_buckets == nullptr) return *this;
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cb(m_buckets[i].data);
            }
        }
        return *this;
    }

    template <typename TDataCb>
    map_type& values(const TDataCb& cb) {
        return const_cast<map_type&>(static_cast<const map_type&>(*this).values(cb));
    }

    template <typename TKVCb>
    const map_type& kvs(const TKVCb& cb) const {
        if (m_buckets == nullptr) return *this;
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cb(m_buckets[i].key, m_buckets[i].data);
            }
        }
        return *this;
    }

    template <typename TKVCb>
    map_type& kvs(const TKVCb& cb) {
        return const_cast<map_type&>(static_cast<const map_type&>(*this).kvs(cb));
    }

private:

    template <typename Data>
    map_type& put_impl(const key_type& key, Data&& data) {
        if (m_len >= detail::lookup_max_load_factor(m_cap)) {
            // Resize based on a load factor of 0.80
            resize();
        }

        auto h = hash(key);
        size_type addr = size_type(h) & (m_cap - 1);
        while (m_buckets[addr].occupied) {
            if (eq(m_buckets[addr].key, key)) {
                m_buckets[addr].data = core::move(data);
                return *this;
            }
            addr = (addr + 1) & (m_cap - 1);
        }
        m_buckets[addr].key = key;
        m_buckets[addr].data = core::move(data);
        m_buckets[addr].occupied = true;
        m_len++;

        return *this;
    }

    void resize() {
        size_type newCap = m_cap < DEFAULT_CAP ? DEFAULT_CAP : m_cap * 2;
        hash_map newMap;
        newMap.m_buckets = reinterpret_cast<bucket*>(core::calloc<allocator_type>(newCap, sizeof(bucket)));
        newMap.m_cap = newCap;
        newMap.m_len = 0;

        if (m_buckets) {
            for (size_type i = 0; i < m_cap; ++i) {
                if (m_buckets[i].occupied) {
                    newMap.put(m_buckets[i].key, core::move(m_buckets[i].data));
                }
            }
        }

        *this = core::move(newMap);
    }

    const bucket* find_bucket(const key_type& key) const {
        auto h = hash(key);
        size_type addr = size_type(h) & (m_cap - 1);
        while (addr < m_cap && m_buckets[addr].occupied) {
            if (eq(m_buckets[addr].key, key)) {
                return &m_buckets[addr];
            }
            addr = (addr + 1) & (m_cap - 1);
        }
        return nullptr;
    }

    bucket* find_bucket(const key_type& key) {
        return const_cast<bucket*>(static_cast<const map_type&>(*this).find_bucket(key));
    }

    bucket* m_buckets;
    size_type m_cap; // The capacity must always be a power of 2.
    size_type m_len;
};

static_assert(core::is_standard_layout_v<hash_map<const char*, i32>>, "hash_map must be standard layout");

template<typename TKey, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct hash_set {
    constexpr static addr_size DEFAULT_CAP = 16;

    using key_type       = TKey;
    using set_type       = hash_set<TKey, TAllocator>;
    using size_type      = addr_size;
    using allocator_type = TAllocator;

    struct bucket {
        key_type key;
        bool occupied;
    };

    static_assert(core::is_pod_v<TKey>, "TKey must be pod type");

    hash_set() : m_buckets(nullptr), m_cap(0), m_len(0) {};
    hash_set(size_type cap) : m_buckets(nullptr), m_cap(core::align_to_pow2(cap)), m_len(0) {
        m_buckets = reinterpret_cast<bucket*>(core::calloc<allocator_type>(m_cap, sizeof(bucket)));
    }
    hash_set(const set_type& other) = delete; // prevent copy ctor
    hash_set(set_type&& other) : m_buckets(other.m_buckets), m_cap(other.m_cap), m_len(other.m_len) {
        if (this == &other) return;
        other.m_buckets = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    ~hash_set() { free(); }

    hash_set& operator=(const set_type& other) = delete; // prevent copy assignment
    hash_set& operator=(size_type) = delete; // prevent size assignment
    hash_set& operator=(set_type&& other) {
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
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                m_buckets[i].occupied = false;
                m_buckets[i].key = key_type();
            }
        }
        m_len = 0;
    }

    void free() {
        if (m_buckets == nullptr) return;
        m_len = 0;
        m_cap = 0;
        core::free<allocator_type>(m_buckets);
        m_buckets = nullptr;
    }

    set_type copy() {
        set_type cpy(m_cap);
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cpy.put(m_buckets[i].key);
            }
        }
        return cpy;
    }

    const key_type* get(const key_type& key) const {
        const bucket* b = find_bucket(key);
        return b ? &b->key : nullptr;
    }

    key_type* get(const key_type& key) {
        return const_cast<key_type*>(static_cast<const set_type &>(*this).get(key));
    }

    set_type& put(const key_type& key) {
        if (m_len >= detail::lookup_max_load_factor(m_cap)) {
            // Resize based on a load factor of 0.80
            resize();
        }

        auto h = hash(key);
        size_type addr = size_type(h) & (m_cap - 1);
        while (m_buckets[addr].occupied) {
            if (eq(m_buckets[addr].key, key)) {
                return *this;
            }
            addr = (addr + 1) & (m_cap - 1);
        }
        m_buckets[addr].key = key;
        m_buckets[addr].occupied = true;
        m_len++;

        return *this;
    }

    set_type& remove(const key_type& key) {
        bucket* b = find_bucket(key);
        if (b) {
            b->occupied = false;
            b->key = key_type(); // rest key
            m_len--;
        }
        return *this;
    }

    template <typename TKeyCb>
    const set_type& keys(const TKeyCb& cb) const {
        if (m_buckets == nullptr) return *this;
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_buckets[i].occupied) {
                cb(m_buckets[i].key);
            }
        }
        return *this;
    }

    template <typename TKeyCb>
    set_type& keys(const TKeyCb& cb) {
        return const_cast<set_type&>(static_cast<const set_type&>(*this).keys(cb));
    }

private:

    void resize() {
        size_type newCap = m_cap < DEFAULT_CAP ? DEFAULT_CAP : m_cap * 2;
        set_type newSet;
        newSet.m_buckets = reinterpret_cast<bucket*>(core::calloc<allocator_type>(newCap, sizeof(bucket)));
        newSet.m_cap = newCap;
        newSet.m_len = 0;

        if (m_buckets) {
            for (size_type i = 0; i < m_cap; ++i) {
                if (m_buckets[i].occupied) {
                    newSet.put(m_buckets[i].key);
                }
            }
        }

        *this = core::move(newSet);
    }

    const bucket* find_bucket(const key_type& key) const {
        auto h = hash(key);
        size_type addr = size_type(h) & (m_cap - 1);
        while (addr < m_cap && m_buckets[addr].occupied) {
            if (eq(m_buckets[addr].key, key)) {
                return &m_buckets[addr];
            }
            addr = (addr + 1) & (m_cap - 1);
        }
        return nullptr;
    }

    bucket* find_bucket(const key_type& key) {
        return const_cast<bucket*>(static_cast<const set_type&>(*this).find_bucket(key));
    }

    bucket* m_buckets;
    size_type m_cap; // The capacity must always be a power of 2.
    size_type m_len;
};

static_assert(core::is_standard_layout_v<hash_set<const char*>>, "hash_set must be standard layout");

} // namespace core
