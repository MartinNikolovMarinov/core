#include <core_system_checks.h>

#if CPU_ARCH_X86_64
    #include "core_asm_x86_64.h"
#elif CPU_ARCH_ARM64
    // TODO2: [ARM SUPPORT] add support for ARM64 at some point
#endif
