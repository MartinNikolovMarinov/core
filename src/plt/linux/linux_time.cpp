#include <plt/core_time.h>

#include <errno.h>
#include <time.h>

#if defined(CPU_ARCH_X86_64) && CPU_ARCH_X86_64 == 1
#include "x86intrin.h"
#endif

namespace core {

u64 getUnixTimestampNowMs() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    u64 res = u64(ts.tv_sec) * 1000 + u64(ts.tv_nsec) / 1000000;
    return res;
}

u64 getMonotonicNowNs() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    u64 res = u64(ts.tv_sec) * 1000000000ULL + u64(ts.tv_nsec);
    return res;
}

u64 getPerfCounter() {
#if defined(CPU_ARCH_X86_64) && CPU_ARCH_X86_64 == 1
    return __rdtsc();
#elif defined(CPU_ARCH_ARM64) && CPU_ARCH_ARM64 == 1
    u64 counter;
    asm volatile("mrs %0, cntvct_el0" : "=r" (counter));
    return counter;
#else
    return getMonotonicNowNs();
#endif
}

u64 getCPUFrequencyHz() {
    static u64 frequency = 0;
    if (frequency > 0) return frequency;

#if defined(CPU_ARCH_ARM64) && (CPU_ARCH_ARM64 == 1)
    // On ARM64, the frequency of the virtual counter is available in CNTFRQ_EL0.
    asm volatile("mrs %0, cntfrq_el0" : "=r" (frequency));
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

    if (elapsedNs == 0) return 1;

    // Calculate frequency in Hz: (tscDiff ticks in elapsedNs nanoseconds).
    frequency = (tscDiff * 1000000000ULL) / elapsedNs;
#endif

    return frequency;
}

} // namespace core
