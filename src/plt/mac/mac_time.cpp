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
    static u64 frequency = 0;
    if (frequency > 0) return frequency;

#if defined(CPU_ARCH_ARM64) && (CPU_ARCH_ARM64 == 1)
    // NOTE: There is no way to actually do this on modern macOS. So just assume 1 Hz for compatibility. 
    //       This will be handled by the getPerformanceCounter
    return 1000000000ULL;
#elif defined(CPU_ARCH_X86_64) && (CPU_ARCH_X86_64 == 1)
    // On x86_64, we calibrate the TSC over a fixed sleep interval.
    u64 start = getMonotonicNowNs();
    u64 tscStart = getPerfCounter();
    timespec sleepTime = { 0, 100000000 }; // 100 ms

    nanosleep(&sleepTime, nullptr);

    u64 end = getMonotonicNowNs();
    u64 tscEnd = getPerfCounter();
    u64 elapsedNs = end - start;
    u64 tscDiff   = tscEnd - tscStart;

    if (elapsedNs == 0) return 0;

    // Calculate frequency in Hz: (tscDiff ticks in elapsedNs nanoseconds).
    frequency = (tscDiff * 1000000000ULL) / elapsedNs;
#endif

    return frequency;
}

} // namespace core
