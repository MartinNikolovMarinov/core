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
#if defined(CPU_ARCH_ARM64) && (CPU_ARCH_ARM64 == 1)
    // On ARM64, the frequency of the virtual counter is available in CNTFRQ_EL0.
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r" (freq));
    return freq;
#elif defined(CPU_ARCH_X86_64) && (CPU_ARCH_X86_64 == 1)
    // On x86_64, we calibrate the TSC over a fixed sleep interval.
    timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
         return 0;
    }
    u64 tscStart = getPerfCounter();
    timespec sleepTime = { 0, 100000000 }; // 100 ms
    nanosleep(&sleepTime, nullptr);
    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
         return 0;
    }
    u64 tscEnd = getPerfCounter();
    u64 elapsedNs = u64(end.tv_sec - start.tv_sec) * 1000000000ULL +
                    u64(end.tv_nsec - start.tv_nsec);
    u64 tscDiff   = tscEnd - tscStart;
    if (elapsedNs == 0) {
         return 0;
    }
    // Calculate frequency in Hz: (tscDiff ticks in elapsedNs nanoseconds).
    u64 frequency = (tscDiff * 1000000000ULL) / elapsedNs;
    return frequency;
#else
    return 0;
#endif
}

} // namespace core
