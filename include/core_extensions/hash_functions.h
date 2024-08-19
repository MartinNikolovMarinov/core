#pragma once

#include <core_hash.h>
#include <core_str_view.h>
#include <core_cstr.h>

namespace core {

using namespace coretypes;

namespace detail {

template <typename TInt>
addr_size hashPrimitiveFnva_64(const TInt& key) {
    return core::fnv1a_64(&key, sizeof(TInt));
}

} // namespace

template<> inline addr_size hash<i8>(const i8& key)             { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<i8>(const i8& a, const i8& b)    { return a == b; }
template<> inline addr_size hash<i16>(const i16& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<i16>(const i16& a, const i16& b) { return a == b; }
template<> inline addr_size hash<i32>(const i32& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<i32>(const i32& a, const i32& b) { return a == b; }
template<> inline addr_size hash<i64>(const i64& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<i64>(const i64& a, const i64& b) { return a == b; }

template<> inline addr_size hash<u8>(const u8& key)             { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<u8>(const u8& a, const u8& b)    { return a == b; }
template<> inline addr_size hash<u16>(const u16& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<u16>(const u16& a, const u16& b) { return a == b; }
template<> inline addr_size hash<u32>(const u32& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<u32>(const u32& a, const u32& b) { return a == b; }
template<> inline addr_size hash<u64>(const u64& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<u64>(const u64& a, const u64& b) { return a == b; }

template<> inline addr_size hash<f32>(const f32& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<f32>(const f32& a, const f32& b) { return a == b; }
template<> inline addr_size hash<f64>(const f64& key)           { return detail::hashPrimitiveFnva_64(key); }
template<> inline bool      eq<f64>(const f64& a, const f64& b) { return a == b; }

template<> inline addr_size hash<StrView>(const StrView& key) {
    addr_size ret = core::fnv1a_64(key.data(), key.len());
    return ret;
}

template<> inline bool eq<StrView>(const StrView& a, const StrView& b) {
    bool ret = core::memcmp(a.data(), a.len(), b.data(), b.len()) == 0;
    return ret;
}

} // namespace core
