#include "intrin.h"

#if COMPILER_MSVC
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace core
{

u64 GetCPUTicks() {
    return __rdtsc();
}

} // namespace core
