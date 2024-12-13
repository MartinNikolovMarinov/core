#pragma once

#include <core_alloc.h>
#include <core_traits.h>
#include <core_types.h>
#include <core_hash.h>

#include <new>

namespace core {

template <typename TKey, typename TValue> struct HashMap;
template <typename TKey, typename TValue> HashMap<TKey, TValue> createHashMap();

namespace detail {

static constexpr f64 DEFAULT_MAX_LOAD_FACTOR = 0.8;

constexpr inline addr_size addLoadFactor(addr_size cap, f64 loadFactor) {
    addr_size ret = cap + addr_size(f64(cap) * loadFactor);
    return ret;
}

// If the size of a hash map is a power of 2, then a lot of the expensive modulo operations can be replaced with bitwise
// AND. This function returns the next power of 2 for a given cap.
constexpr inline addr_size nextPowerOf2ForCap(addr_size cap) {
    for (addr_size i = 0; i < sizeof(addr_size) * 8; i++) {
        const addr_size powOf2 = (1ull << i);
        if (cap <= powOf2) {
            return powOf2;
        }
    }

    Panic(false, "nextPowerOf2: value is not a power of 2");
    return 0;
}

} // namespace detail

template <typename TKey, typename TValue>
struct HashMap {
    using key_type    = TKey;
    using value_type  = TValue;
    using size_type   = addr_size;

    static constexpr f64 maxLoadFactor = detail::DEFAULT_MAX_LOAD_FACTOR;

    enum BucketState: u8 {
        Free,
        Occupied,
        Deleted,
    };

    // IMPORTANT: Only a subset of the features can be used without a copy constructr, but it is allowed.
    //            On the other hand, without a move contructor this struct is useless.
    static_assert(std::is_move_constructible_v<key_type>, "TKey must be move constructible");
    static_assert(std::is_move_constructible_v<value_type>, "TValue must be move constructible");
    static_assert(HashableConcept<key_type>, "TKey must be hashable");

    HashMap() : m_keys(nullptr), m_values(nullptr), m_bucketState(nullptr), m_cap(0), m_len(0) {}
    HashMap(size_type cap) {
        cap = detail::addLoadFactor(cap, maxLoadFactor);
        m_cap = cap > 0 ? detail::nextPowerOf2ForCap(cap) : 0;
        m_len = 0;
        if (m_cap > 0) {
            m_keys        = reinterpret_cast<key_type*>(core::alloc(m_cap, sizeof(key_type)));
            m_values      = reinterpret_cast<value_type*>(core::alloc(m_cap, sizeof(value_type)));
            m_bucketState = reinterpret_cast<BucketState*>(core::zeroAlloc(m_cap, sizeof(BucketState)));
        }
        else {
            m_keys = nullptr;
            m_values = nullptr;
            m_bucketState = nullptr;
        }
    }

    HashMap(const HashMap& other) = delete; // prevent copy ctor
    HashMap(HashMap&& other) {
        if (this == &other) return;

        m_keys = other.m_keys;
        m_values = other.m_values;
        m_bucketState = other.m_bucketState;
        m_cap = other.m_cap;
        m_len = other.m_len;

        other.m_keys = nullptr;
        other.m_values = nullptr;
        other.m_bucketState = nullptr;
        other.m_cap = 0;
        other.m_len = 0;
    }
    ~HashMap() { free(); }

    HashMap& operator=(const HashMap& other) = delete; // prevent copy assignment
    HashMap& operator=(size_type) = delete; // prevent size assignment
    HashMap& operator=(HashMap&& other) {
        if (this == &other) return *this;

        free(); // very important to free before assigning new data.

        m_keys = other.m_keys;
        m_values = other.m_values;
        m_bucketState = other.m_bucketState;
        m_cap = other.m_cap;
        m_len = other.m_len;

        other.m_keys = nullptr;
        other.m_values = nullptr;
        other.m_bucketState = nullptr;
        other.m_cap = 0;
        other.m_len = 0;

        return *this;
    }

    size_type cap()     const { return m_cap; }
    size_type byteCap() const { return m_cap * core::align(sizeof(value_type) + sizeof(key_type) + sizeof(bool)); }
    size_type len()     const { return m_len; }
    size_type byteLen() const { return m_len * core::align(sizeof(value_type) + sizeof(key_type) + sizeof(bool)); }
    bool      empty()   const { return m_len == 0; }

    void clear() {
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_bucketState[i] == BucketState::Occupied) {
                m_values[i].~value_type();
                m_keys[i].~key_type();
            }
            m_bucketState[i] = BucketState::Free;
        }

        m_len = 0;
    }

    void free() {
        if (m_keys == nullptr) return;

        clear();

        core::free(m_keys, m_cap, sizeof(key_type));
        core::free(m_values, m_cap, sizeof(value_type));
        core::free(m_bucketState, m_cap, sizeof(BucketState));

        m_cap = 0;
        m_keys = nullptr;
        m_values = nullptr;
        m_bucketState = nullptr;
    }

    HashMap copy() const {
        value_type* copyData = nullptr;
        key_type* copyKeys = nullptr;
        BucketState* copyBucketState = nullptr;

        if (m_cap > 0) {
            copyData        = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
            copyKeys        = reinterpret_cast<key_type *>(core::alloc(m_cap, sizeof(key_type)));
            copyBucketState = reinterpret_cast<BucketState *>(core::alloc(m_cap, sizeof(BucketState)));

            for (size_type i = 0; i < m_cap; i++) {
                new (&copyData[i]) value_type(m_values[i]);
                new (&copyKeys[i]) key_type(m_keys[i]);
                copyBucketState[i] = m_bucketState[i];
            }
        }

        HashMap result;
        result.m_keys = copyKeys;
        result.m_values = copyData;
        result.m_bucketState = copyBucketState;
        result.m_cap = m_cap;
        result.m_len = m_len;
        return result;
    }

    const value_type* get(const key_type& key) const {
        size_type idx = findIndex(key);
        if (idx < m_cap) {
            return &m_values[idx];
        }

        return nullptr;
    }

    value_type* get(const key_type& key) {
        return const_cast<value_type*>(static_cast<const HashMap&>(*this).get(key));
    }

    bool contains(const key_type& key) const {
        return findIndex(key) < m_cap;
    }

    value_type* put(const key_type& key, const value_type& val) {
        return putImpl<const value_type&>(key, val);
    }
    value_type* put(const key_type& key, value_type&& val) {
        return putImpl<value_type&&>(key, std::move(val));
    }

    bool remove(const key_type& key) {
        if (m_len == 0) {
            return false;
        }

        size_type idx = findIndex(key);
        if (idx < m_cap) {
            m_values[idx].~value_type();
            m_keys[idx].~key_type();
            m_bucketState[idx] = BucketState::Deleted;
            m_len--;
            return true;
        }

        return false;
    }

    void ensureCap(size_type newCap) {
        newCap = detail::addLoadFactor(newCap, maxLoadFactor);
        if (newCap <= m_cap) {
            // There is enough space. Do nothing.
            return;
        }

        newCap = detail::nextPowerOf2ForCap(newCap);

        key_type* newKeys           = reinterpret_cast<key_type*>(core::alloc(newCap, sizeof(key_type)));
        value_type* newValues       = reinterpret_cast<value_type*>(core::alloc(newCap, sizeof(value_type)));
        BucketState* newBucketState = reinterpret_cast<BucketState*>(core::zeroAlloc(newCap, sizeof(BucketState)));

        for (size_type i = 0; i < m_cap; i++) {
            if (m_bucketState[i] == BucketState::Occupied) {
                auto h         = core::hash(m_keys[i]);
                size_type addr = size_type(h) & (newCap - 1); // re-hash the key and find a new slot.

                // Find the first empty slot starting from the calculated slot.
                while (newBucketState[addr]) {
                    // This loop should be guaranteed to terminate because new cap is always larger than the old cap.
                    addr = (addr + 1) & (newCap - 1);
                }

                new (&newKeys[addr]) key_type(std::move(m_keys[i]));
                new (&newValues[addr]) value_type(std::move(m_values[i]));
                newBucketState[addr] = BucketState::Occupied;
            }
        }

        if (m_keys) {
            core::free(m_keys, m_cap, sizeof(key_type));
            core::free(m_values, m_cap, sizeof(value_type));
            core::free(m_bucketState, m_cap, sizeof(BucketState));
        }

        m_cap = newCap;
        m_keys = newKeys;
        m_values = newValues;
        m_bucketState = newBucketState;
    }

    template <typename TCallback>
    void keys(TCallback cb) const {
        for (size_type i = 0; i < m_cap; i++) {
            if (m_bucketState[i] == BucketState::Occupied) {
                if (!cb(m_keys[i])) {
                    return;
                }
            }
        }
    }

    template <typename TCallback>
    void values(TCallback cb) const {
        for (size_type i = 0; i < m_cap; i++) {
            if (m_bucketState[i] == BucketState::Occupied) {
                if (!cb(m_values[i])) {
                    return;
                }
            }
        }
    }

    template <typename TCallback>
    void values(TCallback cb) {
        return static_cast<const HashMap&>(*this).values(cb);
    }

    template <typename TCallback>
    void entries(TCallback cb) const {
        for (size_type i = 0; i < m_cap; i++) {
            if (m_bucketState[i] == BucketState::Occupied) {
                if (!cb(m_keys[i], m_values[i])) {
                    return;
                }
            }
        }
    }

private:

    template <typename TTVal>
    inline value_type* putImpl(const key_type& key, TTVal val) {
        static_assert(std::is_reference_v<TTVal>, "Value must be a reference");

        ensureCap(m_len + 1);

        auto h = core::hash(key);
        size_type addr = size_type(h) & (m_cap - 1);

        while (m_bucketState[addr] == BucketState::Occupied) {
            if (core::eq(m_keys[addr], key)) {
                // Key already exists, replace the value.
                if constexpr (std::is_lvalue_reference_v<TTVal>) {
                    new (&m_values[addr]) value_type(val);
                }
                else {
                    new (&m_values[addr]) value_type(std::move(val));
                }
                return &m_values[addr];
            }
            addr = (addr + 1) & (m_cap - 1);
        }

        // Slot is empty

        // Insert key
        new (&m_keys[addr]) key_type(key);

        // Insert value
        if constexpr (std::is_lvalue_reference_v<TTVal>) {
            new (&m_values[addr]) value_type(val);
        }
        else {
            new (&m_values[addr]) value_type(std::move(val));
        }

        // Mark the slot as occupied
        m_bucketState[addr] = BucketState::Occupied;
        m_len++;

        return &m_values[addr];
    }

    inline size_type findIndex(const key_type& key) const {
        if (m_len == 0) {
            return m_cap;
        }

        auto h                = core::hash(key);
        size_type addr        = size_type(h) & (m_cap - 1);
        size_type startAddr   = addr;

        while (addr < m_cap && m_bucketState[addr] != BucketState::Free) {
            if (core::eq(m_keys[addr], key)) {
                return addr;
            }

            addr = (addr + 1) & (m_cap - 1);
            if (addr == startAddr) [[unlikely]] {
                // Full loop around the table.
                // This happens when the table is filled to capacity and the loop goes forever.
                // This is a problem only when m_cap == m_len.
                break;
            }
        }

        return m_cap;
    }

    key_type* m_keys;
    value_type* m_values;
    BucketState* m_bucketState;
    size_type m_cap;
    size_type m_len;
};

template <typename TKey, typename TValue>
HashMap<TKey, TValue> createHashMap() {
    constexpr addr_size DEFAULT_CAP = 16;
    return HashMap<TKey, TValue>(DEFAULT_CAP);
}

// TODO: Implement Hash Set.
// TODO: Implement a static sized hash set for the cases where a exactly sized map is needed.

} // namespace core
