#pragma once

#if defined(__clang__)
    #define COMPILER_CLANG 1
#else
    #define COMPILER_CLANG 0
#endif
#if (defined(__GNUC__) || defined(__GNUG__)) && COMPILER_CLANG == 0
    #define COMPILER_GCC 1
#else
    #define COMPILER_GCC 0
#endif
#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
#else
    #define COMPILER_MSVC 0
#endif
#if COMPILER_CLANG == 0 && COMPILER_GCC == 0 && COMPILER_MSVC == 0
    #define COMPILER_UNKNOWN 1
#else
    #define COMPILER_UNKNOWN 0
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
    #define OS_WIN 1
#else
    #define OS_WIN 0
#endif
#if defined(__linux__)
    #define OS_LINUX 1
#else
    #define OS_LINUX 0
#endif
#if defined(__APPLE__) || defined(__MACH__)
    #define OS_MAC 1
#else
    #define OS_MAC 0
#endif
#if OS_WIN == 0 && OS_LINUX == 0 && OS_MAC == 0
    #define OS_UNKNOWN 1
#else
    #define OS_UNKNOWN 0
#endif

// Most of these are unsupported, but the detection code is good.
// Credit to FreakAnon's answer here: https://stackoverflow.com/a/66249936 .
#if defined(__x86_64__) || defined(_M_X64)
    #define CPU_ARCH "x86_64";
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    #define CPU_ARCH "x86_32";
#elif defined(__ARM_ARCH_2__)
    #define CPU_ARCH "ARM2";
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
    #define CPU_ARCH "ARM3";
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
    #define CPU_ARCH "ARM4T";
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
    #define CPU_ARCH "ARM5"
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
    #define CPU_ARCH "ARM6T2";
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
    #define CPU_ARCH "ARM6";
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    #define CPU_ARCH "ARM7";
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    #define CPU_ARCH "ARM7A";
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    #define CPU_ARCH "ARM7R";
#elif defined(__ARM_ARCH_7M__)
    #define CPU_ARCH "ARM7M";
#elif defined(__ARM_ARCH_7S__)
    #define CPU_ARCH "ARM7S";
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define CPU_ARCH "ARM64";
#elif defined(mips) || defined(__mips__) || defined(__mips)
    #define CPU_ARCH "MIPS";
#elif defined(__sh__)
    #define CPU_ARCH "SUPERH";
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
    #define CPU_ARCH "POWERPC";
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
    #define CPU_ARCH "POWERPC64";
#elif defined(__sparc__) || defined(__sparc)
    #define CPU_ARCH "SPARC";
#elif defined(__m68k__)
    #define CPU_ARCH "M68K";
#else
    #define CPU_ARCH "UNKNOWN";
#endif
