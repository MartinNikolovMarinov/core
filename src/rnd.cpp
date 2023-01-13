#include <rnd.h>
#include <intrinsic.h>
#include <char_ptr.h>

namespace core {

/**
 * The Algorithm used for pseudo number generation was discovered by Marsaglia in his paper "Xorshift RNGs".
 * The paper is freely available and the algorithm itself is based on shift-register generators.
 * This perticular version does not guarantee the best entropy, it's very simple to implement and incredibly fast.
 *
 * TODO2: If there is a need for generating random bits, in really large arrays, there is a version of xorshift-ing
 *       called "xorwow", which is used by default in Nvidia's CUDA toolkit.
*/

namespace {

u64 seedU64;
const char alphaChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const i32 alphaCharsLen = sizeof(alphaChars) - 1; // alphaChars includes terminating '\0'

u64 xor_shift64(u64 state) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

}

void rnd_init() {
    seedU64 = get_cpu_ticks();
}
void rnd_init(u64 seed) {
    seedU64 = seed;
}

u32 rnd_u32() {
    seedU64 = xor_shift64(seedU64);
    return (u32)seedU64;
}
u32 rnd_u32(u32 min, u32 max) {
    if (min == max) return min;
    u32 ret = (rnd_u32() % (max - min))  + min;
    return ret;
}

u64 rnd_u64() {
    seedU64 = xor_shift64(seedU64);
    return seedU64;
}
u64 rnd_u64(u64 min, u64 max) {
    if (min == max) return min;
    u64 ret = (rnd_u64() % (max - min))  + min;
    return ret;
}

i32 rnd_i32()                 { return (i32)rnd_u32(); }
i32 rnd_i32(i32 min, i32 max) { return (i32)rnd_u32((u32)min, (u32)max); }

i64 rnd_i64()                 { return (i64)rnd_u64(); }
i64 rnd_i64(i64 min, i64 max) { return (i64)rnd_u64((u64)min, (u64)max); }

void rnd_cptr(char* out, ptr_size len) {
    ptr_size i;
    for (i = 0; i < len; i++) {
        i32 randIndex = (u32)rnd_u32() % alphaCharsLen;
        out[i] = alphaChars[randIndex];
    }
    out[i] = '\0';
}

} // namespace core
