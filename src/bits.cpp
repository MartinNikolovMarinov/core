#include "bits.h"
#include <mem.h>

namespace core {

namespace {

template <typename TFloat>
void _float_to_bin(u8 bytes[sizeof(TFloat)], TFloat v) {
    union { TFloat a; u8 bytes[sizeof(TFloat)]; } floatUnion;
    floatUnion.a = v;
    core::memcopy(bytes, floatUnion.bytes, sizeof(TFloat));
}

} // namespace detail

void float_to_bin(u8 bytes[sizeof(f32)], f32 v) { return _float_to_bin(bytes, v); }
void float_to_bin(u8 bytes[sizeof(f64)], f64 v) { return _float_to_bin(bytes, v); }

} // namespace core
