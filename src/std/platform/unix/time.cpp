#include "../plt.h"

#include <errno.h>
#include <sys/time.h>
#include <unistd.h> // for usleep

namespace core::plt
{

PltErrValue<u64> OsUnixTimeStampInMs() {
    struct timeval now;
    i64 ret = gettimeofday(&now, 0);
    if (ret < 0) {
        return { 0, errno };
    }

    u64 timeInMs = ((u64) now.tv_sec) * 1000 + ((u64) now.tv_usec) / 1000;
    return { std::move(timeInMs), 0 };
}

void OsThreadSleep(u64 ms) {
    usleep(ms * 1000);
}

} // namespace core::plt
