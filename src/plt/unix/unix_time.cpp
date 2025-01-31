#include <plt/core_time.h>

#include <errno.h>
#include <time.h>

#if defined(OS_MAC) && OS_MAC == 1
#include <mach/mach_time.h>
#endif

namespace core {

expected<u64, PltErrCode> getCurrentUnixTimestampMs() {
    timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return core::unexpected(PltErrCode(errno));
    }
    u64 res = u64(ts.tv_sec) * 1000 + u64(ts.tv_nsec) / 1000000;
    return res;
}

u64 getPerfCounter() {
#if defined(OS_LINUX) && OS_LINUX == 1
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    u64 res = u64(ts.tv_sec) * 1000000000ULL + u64(ts.tv_nsec);
    return res;
#elif defined(OS_MAC) && OS_MAC == 1
    static mach_timebase_info_data_t timebase = {0, 0};
    if (timebase.denom == 0) {
        mach_timebase_info(&timebase);
    }
    u64 ret = (mach_absolute_time() * timebase.numer) / timebase.denom; // Scaled to nanoseconds
    return ret;
#endif
}

} // namespace core
