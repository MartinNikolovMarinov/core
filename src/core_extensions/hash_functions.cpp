#include <core_extensions/hash_functions.h>

#include <core_hash.h>

namespace core {

using namespace coretypes;

namespace {

template <typename TInt>
addr_size hashIntFnva_64(const TInt& key) {
    return core::fnv1a_64(&key, sizeof(TInt));
}

} // namespace

template<> addr_size hash(const i8& key)            { return hashIntFnva_64(key); }
template<> bool      eq(const i8& a, const i8& b)   { return a == b; }
template<> addr_size hash(const i16& key)           { return hashIntFnva_64(key); }
template<> bool      eq(const i16& a, const i16& b) { return a == b; }
template<> addr_size hash(const i32& key)           { return hashIntFnva_64(key); }
template<> bool      eq(const i32& a, const i32& b) { return a == b; }
template<> addr_size hash(const i64& key)           { return hashIntFnva_64(key); }
template<> bool      eq(const i64& a, const i64& b) { return a == b; }

template<> addr_size hash(const u8& key)            { return hashIntFnva_64(key); }
template<> bool      eq(const u8& a, const u8& b)   { return a == b; }
template<> addr_size hash(const u16& key)           { return hashIntFnva_64(key); }
template<> bool      eq(const u16& a, const u16& b) { return a == b; }
template<> addr_size hash(const u32& key)           { return hashIntFnva_64(key); }
template<> bool      eq(const u32& a, const u32& b) { return a == b; }
template<> addr_size hash(const u64& key)           { return hashIntFnva_64(key); }
template<> bool      eq(const u64& a, const u64& b) { return a == b; }

} // namespace core
