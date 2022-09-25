#include <plt.h>

#include <windows.h>
#include <errhandlingapi.h>

namespace core::plt
{

PltErrValue<u64> OsUnixTimeStampInMs() {
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);

    // The fileTime struct is a 64-bit value representing the number
    // of 100-nanosecond intervals since January 1, 1601 (UTC). For some reason.
    FILETIME fileTime;
    BOOL ret = SystemTimeToFileTime(&systemTime, &fileTime);
    if (ret != 1)  {
        return { 0, GetLastError() };
    }

    u64 timeNowIn100thNanoseconds;
    timeNowIn100thNanoseconds = ((u64)fileTime.dwLowDateTime );
    timeNowIn100thNanoseconds += ((u64)fileTime.dwHighDateTime) << 32;

    // NOTE:
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    const u64 START_OF_EPOCH_IN_NS = 116444736000000000ULL;

    // We subtract the EPOCH to get the Unix timestamp since 1970:
    timeNowIn100thNanoseconds -= START_OF_EPOCH_IN_NS;

    // NOTE:
    // To convert the strange time now in 100th of nanoseconds we divide by 10000 !
    // How very magical, yet somehow actually correct.
    u64 timeNowMs = timeNowIn100thNanoseconds / 10000;
    return { std::move(timeNowMs), 0 };
}

void OsThreadSleep(u64 ms) {
    Sleep((DWORD)ms);
}

} // namespace core::plt
