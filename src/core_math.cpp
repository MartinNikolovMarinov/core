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

FP_Classification fpclassify(f32 x) {
    union { f32 f; u32 i; } u = { x };
    i32 e = (u.i >> 23) & 0xff;
    if (!e) return u.i << 1 ? FP_Classification::SUBNORMAL : FP_Classification::ZERO;
    if (e == 0xff) return u.i << 9 ? FP_Classification::NAN : FP_Classification::INFINITE;
    return FP_Classification::NORMAL;
}

FP_Classification fpclassify(f64 x) {
    union { f64 f; u64 i; } u = { x };
    i32 e = (u.i >> 52) & 0x7ff;
    if (!e) return u.i << 1 ? FP_Classification::SUBNORMAL : FP_Classification::ZERO;
    if (e == 0x7ff) return u.i << 12 ? FP_Classification::NAN : FP_Classification::INFINITE;
    return FP_Classification::NORMAL;
}

bool isinf(f32 x) { return fpclassify(x) == FP_Classification::INFINITE; }
bool isinf(f64 x) { return fpclassify(x) == FP_Classification::INFINITE; }

bool isnan(f32 x) { return fpclassify(x) == FP_Classification::NAN; }
bool isnan(f64 x) { return fpclassify(x) == FP_Classification::NAN; }

bool isnormal(f32 x) { return fpclassify(x) == FP_Classification::NORMAL; }
bool isnormal(f64 x) { return fpclassify(x) == FP_Classification::NORMAL; }

} // namespace core
