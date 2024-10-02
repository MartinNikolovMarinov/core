#include <core_system_checks.h>

#if CPU_ARCH_X86_64 && (COMPILER_CLANG == 1 || COMPILER_GCC == 1)
    #include "core_clang_gcc_asm_x86_64.h"
#elif CPU_ARCH_X86_64 && COMPILER_MSVC == 1
    #include "core_msvc_asm_x86_64.h"
#elif CPU_ARCH_ARM64
    // TODO2: [ARM SUPPORT] add support for ARM64 at some point
#endif
