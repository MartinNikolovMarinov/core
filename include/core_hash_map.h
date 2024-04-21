#pragma once

#include <core_alloc.h>
#include <core_traits.h>
#include <core_types.h>
#include <core_hash.h>

#include <new>

namespace core {

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

    static_assert(HashableConcept<key_type>, "TKey must be hashable");
    static_assert(std::is_move_constructible_v<key_type>, "TKey must be move constructible");
    static_assert(std::is_move_constructible_v<value_type>, "TValue must be move constructible");

    // IMPORTANT: Only a subset of the features can be used without a copy constructr, but it is allowed.
    //            On the other hand, without a move contructor this struct is useless.

    HashMap() : m_keys(nullptr), m_values(nullptr), m_occupied(nullptr), m_cap(0), m_len(0) {}
    HashMap(size_type cap) {
        cap = detail::addLoadFactor(cap, maxLoadFactor);
        m_cap = detail::nextPowerOf2ForCap(cap);
        m_len = 0;
        if (m_cap > 0) {
            m_keys        = reinterpret_cast<key_type*>(core::alloc(m_cap, sizeof(key_type)));
            m_values      = reinterpret_cast<value_type*>(core::alloc(m_cap, sizeof(value_type)));
            m_occupied    = reinterpret_cast<bool*>(core::zeroAlloc(m_cap, sizeof(bool)));
        }
        else {
            m_keys = nullptr;
            m_values = nullptr;
            m_occupied = nullptr;
        }
    }

    HashMap(const HashMap& other) = delete; // prevent copy ctor
    HashMap(HashMap&& other) {
        if (this == &other) return;

        m_keys = other.m_keys;
        m_values = other.m_values;
        m_occupied = other.m_occupied;
        m_cap = other.m_cap;
        m_len = other.m_len;

        other.m_keys = nullptr;
        other.m_values = nullptr;
        other.m_occupied = nullptr;
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
        m_occupied = other.m_occupied;
        m_cap = other.m_cap;
        m_len = other.m_len;

        other.m_keys = nullptr;
        other.m_values = nullptr;
        other.m_occupied = nullptr;
        other.m_cap = 0;
        other.m_len = 0;

        return *this;
    }

    size_type cap()     const { return m_cap; }
    size_type byteCap() const { return m_cap * sizeof(value_type) * sizeof(key_type) * sizeof(bool); }
    size_type len()     const { return m_len; }
    size_type byteLen() const { return m_len * sizeof(value_type) * sizeof(key_type) * sizeof(bool); }
    bool      empty()   const { return m_len == 0; }

    void clear() {
        for (size_type i = 0; i < m_cap; ++i) {
            if (m_occupied[i]) {
                m_values[i].~value_type();
                m_keys[i].~key_type();
            }
        }

        m_len = 0;
    }

    void free() {
        if (m_keys == nullptr) return;

        clear();

        core::free(m_keys, m_cap, sizeof(key_type));
        core::free(m_values, m_cap, sizeof(value_type));
        core::free(m_occupied, m_cap, sizeof(bool));

        m_cap = 0;
        m_keys = nullptr;
        m_values = nullptr;
        m_occupied = nullptr;
    }

    HashMap copy() {
        value_type* copyData = nullptr;
        key_type* copyKeys = nullptr;
        bool* copyOccupied = nullptr;

        if (m_cap > 0) {
            copyData     = reinterpret_cast<value_type *>(core::alloc(m_cap, sizeof(value_type)));
            copyKeys     = reinterpret_cast<key_type *>(core::alloc(m_cap, sizeof(key_type)));
            copyOccupied = reinterpret_cast<bool *>(core::alloc(m_cap, sizeof(bool)));

            for (size_type i = 0; i < m_len; i++) {
                new (&copyData[i]) value_type(m_values[i]);
                new (&copyKeys[i]) key_type(m_keys[i]);
                copyOccupied[i] = m_occupied[i];
            }
        }

        HashMap result;
        result.m_keys = copyKeys;
        result.m_values = copyData;
        result.m_occupied = copyOccupied;
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

    value_type* put(const key_type& key, const value_type& val) {
        return putImpl<const key_type&, const value_type&>(key, val);
    }
    value_type* put(const key_type& key, value_type&& val) {
        return putImpl<const key_type&, value_type&&>(key, std::move(val));
    }
    value_type* put(key_type&& key, const value_type& val) {
        return putImpl<key_type&&, const value_type&>(std::move(key), val);
    }
    value_type* put(key_type&& key, value_type&& val) {
        return putImpl<key_type&&, value_type&&>(std::move(key), std::move(val));
    }

    bool remove(const key_type& key) {
        if (m_len == 0) {
            return false;
        }

        size_type idx = findIndex(key);
        if (idx < m_cap) {
            m_values[idx].~value_type();
            m_keys[idx].~key_type();
            m_occupied[idx] = false;
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

        key_type* newKeys     = reinterpret_cast<key_type*>(core::alloc(newCap, sizeof(key_type)));
        value_type* newValues = reinterpret_cast<value_type*>(core::alloc(newCap, sizeof(value_type)));
        bool* newOccupied     = reinterpret_cast<bool*>(core::zeroAlloc(newCap, sizeof(bool)));

        for (size_type i = 0; i < m_cap; i++) {
            if (m_occupied[i]) {
                auto h         = core::hash(m_keys[i]);
                size_type addr = size_type(h) & (newCap - 1);

                // Find an empty slot.
                while (newOccupied[addr]) {
                    addr = (addr + 1) & (newCap - 1);
                }

                new (&newKeys[addr]) key_type(std::move(m_keys[i]));
                new (&newValues[addr]) value_type(std::move(m_values[i]));
                newOccupied[addr] = true;
            }
        }

        if (m_keys) {
            core::free(m_keys, m_cap, sizeof(key_type));
            core::free(m_values, m_cap, sizeof(value_type));
            core::free(m_occupied, m_cap, sizeof(bool));
        }

        m_cap = newCap;
        m_keys = newKeys;
        m_values = newValues;
        m_occupied = newOccupied;
    }

    template <typename TCallback>
    void keys(TCallback cb) const {
        for (size_type i = 0; i < m_cap; i++) {
            if (m_occupied[i]) {
                cb(m_keys[i]);
            }
        }
    }

    template <typename TCallback>
    void values(TCallback cb) const {
        for (size_type i = 0; i < m_cap; i++) {
            if (m_occupied[i]) {
                cb(m_values[i]);
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
            if (m_occupied[i]) {
                cb(m_keys[i], m_values[i]);
            }
        }
    }

private:

    template <typename TTKey, typename TTVal>
    inline value_type* putImpl(TTKey key, TTVal val) {
        static_assert(std::is_reference_v<TTKey>, "Key must be a reference");
        static_assert(std::is_reference_v<TTVal>, "Value must be a reference");

        ensureCap(m_len + 1);

        auto h = core::hash(key);
        size_type addr = size_type(h) & (m_cap - 1);

        while (m_occupied[addr]) {
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

        // Slot is empty, insert the key and value.

        if constexpr (std::is_lvalue_reference_v<TTKey>) {
            new (&m_keys[addr]) key_type(key);
        }
        else {
            new (&m_keys[addr]) key_type(std::move(key));
        }

        if constexpr (std::is_lvalue_reference_v<TTVal>) {
            new (&m_values[addr]) value_type(val);
        }
        else {
            new (&m_values[addr]) value_type(std::move(val));
        }

        m_occupied[addr] = true;
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

        while (addr < m_cap && m_occupied[addr]) {
            if (core::eq(m_keys[addr], key)) {
                return addr;
            }

            addr = (addr + 1) & (m_cap - 1);
            if (addr == startAddr) {
                // Full loop around the table.
                // This happens when the table is filled to capacity and the loop goes forever.
                // This is a problem only for small tables.
                break;
            }
        }

        return m_cap;
    }

    key_type* m_keys;
    value_type* m_values;
    bool* m_occupied;
    size_type m_cap;
    size_type m_len;
};

} // namespace core
