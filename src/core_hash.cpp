#include <core_hash.h>

namespace core {

u64 fnv1a_64(const void* input, addr_size len, u64 seed) {
    // Hash parameters for 64-bit FNV-1a:
    constexpr u64 FNV_PRIME = 0x00000100000001B3;
    constexpr u64 FNV_OFFSET_BASIS = 0xcbf29ce484222325;

    u64 hash = FNV_OFFSET_BASIS ^ seed;
    const u8* p = reinterpret_cast<const u8*>(input);

    for (addr_size i = 0; i < len; i++) {
        hash ^= p[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

u64 djb2_64(const void* input, addr_size len, u64 seed) {
    constexpr u64 DJB2_INITIAL_CONSTANT = 5381;

    u64 hash = DJB2_INITIAL_CONSTANT ^ seed;
    const u8* p = reinterpret_cast<const u8*>(input);

    for (addr_size i = 0; i < len; i++) {
        u8 c = p[i];
        hash = ((hash << 5) + hash) ^ c; /* hash * 33 ^ c */
    }

    return hash;
}

} // namespace core
