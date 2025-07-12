#include <plt/core_time.h>

#include <windows.h>
#include <intrin.h> // for __rdtsc

namespace core {

u64 getUnixTimestampNowMs() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    // FILETIME is in 100-nanosecond intervals since January 1, 1601 (UTC).
    ULARGE_INTEGER ull;
    ull.LowPart  = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;

    // Offset between 1601 and 1970 in 100-ns units.
    constexpr u64 EPOCH_DIFF = 116444736000000000ULL;
    ull.QuadPart -= EPOCH_DIFF;

    return ull.QuadPart / 10000; // Convert to milliseconds
}

u64 getMonotonicNowNs() {
    LARGE_INTEGER counter, freq;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);

    // Split into whole seconds and remainder to avoid overflow.
    u64 sec = counter.QuadPart / freq.QuadPart;
    u64 rem = counter.QuadPart % freq.QuadPart;
    u64 ns = sec * 1000000000ULL + (rem * 1000000000ULL) / freq.QuadPart;

    return ns;
}

// Returns a high-resolution performance counter value.
// For x86_64, we use __rdtsc(); for other architectures we fall back to the monotonic counter.
u64 getPerfCounter() {
#if defined(CPU_ARCH_X86_64) && (CPU_ARCH_X86_64 == 1)
    return __rdtsc();
#elif defined(CPU_ARCH_ARM64) && (CPU_ARCH_ARM64 == 1)
    // On Windows ARM, there's no direct equivalent to __rdtsc.
    return getMonotonicNowNs();
#else
    return getMonotonicNowNs();
#endif
}

// Returns the estimated CPU frequency in Hz.
// On x86_64 this function calibrates the TSC by measuring over a fixed 100ms Sleep interval.
// On Windows ARM, we use QueryPerformanceFrequency as an approximation.
u64 getCPUFrequencyHz() {
    static u64 frequency = 0;
    if (frequency > 0) return frequency;

#if defined(CPU_ARCH_X86_64) && (CPU_ARCH_X86_64 == 1)
    LARGE_INTEGER start, end, perfFreq;
    QueryPerformanceCounter(&start);
    u64 tscStart = getPerfCounter();

    Sleep(100); // Sleep for 100 ms.

    QueryPerformanceCounter(&end);
    u64 tscEnd = getPerfCounter();
    QueryPerformanceFrequency(&perfFreq);

    // Calculate elapsed time in nanoseconds.
    u64 elapsedNs = ((end.QuadPart - start.QuadPart) * 1000000000ULL) / perfFreq.QuadPart;
    u64 tscDiff   = tscEnd - tscStart;

    if (elapsedNs == 0) return 0;

    // Compute frequency: (tsc ticks * 1e9) / elapsed time in ns.
    frequency = (tscDiff * 1000000000ULL) / elapsedNs;
#elif defined(CPU_ARCH_ARM64) && (CPU_ARCH_ARM64 == 1)
    // As a fallback on Windows ARM, use the performance counter frequency.
    LARGE_INTEGER perfFreq;
    QueryPerformanceFrequency(&perfFreq);
    frequency = u64(perfFreq.QuadPart);
#endif

    return frequency;
}


}
