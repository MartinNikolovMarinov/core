#include <plt/core_time.h>

#include <sys/time.h>       // for gettimeofday
#include <time.h>           // for nanosleep and struct timespec
#include <mach/mach_time.h> // for mach_absolute_time and mach_timebase_info
#include <sys/sysctl.h>     // for sysctl (to get CPU frequency on ARM64)

#if defined(CPU_ARCH_X86_64) && CPU_ARCH_X86_64 == 1
    #include <x86intrin.h>  // for __rdtsc()
#endif

namespace core {

u64 getUnixTimestampNowMs() {
    // gettimeofday returns seconds and microseconds since the Unix epoch.
    timeval tv;
    gettimeofday(&tv, nullptr);
    return u64(tv.tv_sec) * 1000ULL + u64(tv.tv_usec) / 1000ULL;
}

u64 getMonotonicNowNs() {
    // mach_absolute_time() returns time in arbitrary units.
    // mach_timebase_info converts that to nanoseconds.
    static mach_timebase_info_data_t timebaseInfo = {0, 0};
    if (timebaseInfo.denom == 0) {
        mach_timebase_info(&timebaseInfo);
    }
    u64 absTime = mach_absolute_time();
    return (absTime * timebaseInfo.numer) / timebaseInfo.denom;
}

u64 getPerfCounter() {
#if defined(CPU_ARCH_X86_64) && (CPU_ARCH_X86_64 == 1)
    return __rdtsc();
#elif defined(CPU_ARCH_ARM64) && (CPU_ARCH_ARM64 == 1)
    // On ARM64, no direct equivalent exists; use the monotonic counter.
    return getMonotonicNowNs();
#else
    return getMonotonicNowNs();
#endif
}

u64 getCPUFrequencyHz() {
#if defined(CPU_ARCH_X86_64) && (CPU_ARCH_X86_64 == 1)
    // Calibrate the TSC by measuring over a 100ms interval.
    u64 start = getMonotonicNowNs();
    u64 tscStart = getPerfCounter();
    timespec sleepTime = { 0, 100000000 }; // 100 ms

    nanosleep(&sleepTime, nullptr);

    u64 end = getMonotonicNowNs();
    u64 tscEnd = getPerfCounter();
    u64 elapsedNs = end - start;
    u64 tscDiff   = tscEnd - tscStart;

    if (elapsedNs == 0) return 0;
    // Compute frequency in Hz: (tsc ticks * 1e9) / elapsed nanoseconds.

    u64 frequency = (tscDiff * 1000000000ULL) / elapsedNs;
    return frequency;
#elif defined(CPU_ARCH_ARM64) && (CPU_ARCH_ARM64 == 1)
    // Use sysctl to query the CPU frequency.
    u64 freq = 0;
    size_t size = sizeof(freq);
    int mib[2] = { CTL_HW, HW_CPU_FREQ };
    if (sysctl(mib, 2, &freq, &size, nullptr, 0) == 0)
        return freq;
    // Fallback: treat mach_absolute_time() as nanoseconds.
    return 1000000000ULL;
#else
    return 0;
#endif
}

} // namespace core
