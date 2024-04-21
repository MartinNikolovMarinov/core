#pragma once

#include <core_hash.h>

namespace core {

using namespace coretypes;

template<> addr_size hash(const i8& key);
template<> bool      eq(const i8& a, const i8& b);
template<> addr_size hash(const i16& key);
template<> bool      eq(const i16& a, const i16& b);
template<> addr_size hash(const i32& key);
template<> bool      eq(const i32& a, const i32& b);
template<> addr_size hash(const i64& key);
template<> bool      eq(const i64& a, const i64& b);

template<> addr_size hash(const u8& key);
template<> bool      eq(const u8& a, const u8& b);
template<> addr_size hash(const u16& key);
template<> bool      eq(const u16& a, const u16& b);
template<> addr_size hash(const u32& key);
template<> bool      eq(const u32& a, const u32& b);
template<> addr_size hash(const u64& key);
template<> bool      eq(const u64& a, const u64& b);


} // namespace core
