#include <core_system_checks.h>

#if defined(CPU_ARCH_X86_64) && CPU_ARCH_X86_64 == 1
    #include "core_asm_x86_64.h"
#elif CPU_ARCH_ARM64
    // TODO2: [ARM SUPPORT] add support for ARM64 at some point
#endif
