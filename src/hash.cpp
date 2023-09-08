#include "hash.h"

namespace core {

u32 simple_hash(const void* input, addr_size len) {
    const u32 prime = 397;
    u32 h32 = 2166136261U;
    const u8* p = reinterpret_cast<const u8*>(input);

    for (addr_size i = 0; i < len; i++) {
        u8 c = p[i];
        h32 = (h32 ^ c) * prime;
    }

    return h32;
}

} // namespace core
