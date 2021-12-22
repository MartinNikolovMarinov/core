#include "core.h"

namespace core
{

/**
 * The Algorithm used for pseudo number generation was discovered by Marsaglia in his paper "Xorshift RNGs".
 * The paper is freely available and the algorithm itself is based on shift-register generators.
 * This perticular version does not guarantee the best entropy, it's very simple to implement and incredibly fast.
 *
 * TODO2: If there is a need for generating random bits, in really large arrays, there is a version of xorshift-ing
 *       called "xorwow", which is used by default in Nvidia's CUDA toolkit.
*/

namespace
{

u64 seedU64;
const char alphaChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const i32 alphaCharsLen = sizeof(alphaChars) - 1; // alphaChars includes terminating '\0'

u32 XORShift32(u32 state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

u64 XORShift64(u64 state) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

}

void RandInit() {
    seedU64 = GetCPUTicks();
}
void RandInit(u64 seed) {
    seedU64 = seed;
}

u32 RandU32() {
    seedU64 = XORShift64(seedU64);
    return (u32)seedU64;
}
u32 RandU32(u32 min, u32 max) {
    if (min == max) return min;
    u32 ret = (RandU32() % (max - min))  + min;
    return ret;
}

u64 RandU64() {
    seedU64 = XORShift64(seedU64);
    return seedU64;
}
u64 RandU64(u64 min, u64 max) {
    if (min == max) return min;
    u64 ret = (RandU64() % (max - min))  + min;
    return ret;
}

i32 RandI32()                 { return (i32)RandU32(); }
i32 RandI32(i32 min, i32 max) { return (i32)RandU32((u32)min, (u32)max); }

i64 RandI64()                 { return (i64)RandU64(); }
i64 RandI64(i64 min, i64 max) { return (i64)RandU64((u64)min, (u64)max); }

void RandCptr(char* out, ptr_size len) {
    ptr_size i;
    for (i = 0; i < len; i++) {
        i32 randIndex = (u32)RandU32() % alphaCharsLen;
        out[i] = alphaChars[randIndex];
    }
    out[i] = '\0';
}

} // namespace core
