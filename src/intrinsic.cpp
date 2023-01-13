#include <intrinsic.h>

#if COMPILER_MSVC
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace core {

u64 get_cpu_ticks() {
    return __rdtsc();
}

} // namespace core
