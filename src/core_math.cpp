#include <core_math.h>

namespace core {

u32 exponent(f32 n) {
    union { f32 f; u32 i; } u = { n };
    u32 ret = (u32)(u.i >> 23 & 0xff);
    return ret;
}

u32 exponent(f64 n) {
    union { f64 f; i64 i; } u = { n };
    u32 ret = (u32)(u.i >> 52 & 0x7ff);
    return ret;
}

u32 mantissa(f32 n) {
    union { f32 f; u32 i; } u = { n };
    u32 ret = (u32)(u.i & 0x7fffff);
    return ret;
}

u64 mantissa(f64 n) {
    union { f64 f; u64 i; } u = { n };
    u64 ret = (u64)(u.i & 0xfffffffffffff);
    return ret;
}

} // namespace core
