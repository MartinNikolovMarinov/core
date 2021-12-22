#include "../core.h"

#include <errno.h>
#include <sys/time.h>
#include <unistd.h> // for usleep

namespace core
{

Optional<u64, OSErrCode> GetUnixTimeNow()
{
    struct timeval now;
    i64 ret = gettimeofday(&now, null);
    if (ret < 0) {
        return Optional<u64, OSErrCode>(0, errno);
    }

    u64 timeInMs = ((u64) now.tv_sec) * 1000 + ((u64) now.tv_usec) / 1000;
    return Optional<u64, OSErrCode>(timeInMs, null);
}

void ThreadSleep(u64 _ms)
{
    usleep(_ms * 1000);
}

} // namespace core
