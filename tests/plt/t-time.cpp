#include "../t-index.h"

#include <chrono>
#include <thread>

i32 unixTimestampTest() {
    using namespace std::chrono_literals;

    u64 ts = core::getUnixTimestampNowMs();
    auto stdNow = std::chrono::system_clock::now();

    // Should be in the ballpark of what the standard library returns.
    {
        u64 stdTs = u64(std::chrono::duration_cast<std::chrono::milliseconds>(
            stdNow.time_since_epoch()).count());
        u64 diff =  ts > stdTs ? ts - stdTs : stdTs - ts;
        CT_CHECK(diff < 50);
    }

    std::this_thread::sleep_for(50ms);

    u64 elapsed = core::getUnixTimestampNowMs() - ts;
    u64 stdElapsed = u64(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - stdNow).count());

    u64 diff =  elapsed > stdElapsed ? elapsed - stdElapsed : stdElapsed - elapsed;
    CT_CHECK(diff < 20);

    return 0;
}

i32 monotonicTimestampTest() {
    using namespace std::chrono_literals;

    u64 ts = core::getMonotonicNowNs();
    auto stdNow = std::chrono::steady_clock::now();

    // Should be in very close to what the standard library returns
    {
        u64 stdTs = u64(std::chrono::duration_cast<std::chrono::nanoseconds>(
            stdNow.time_since_epoch()).count());
        u64 diff = ts > stdTs ? ts - stdTs : stdTs - ts;
        // Allow a tolerance of 1 millisecond (1,000,000 ns)
        CT_CHECK(diff < 1000000);
    }

    std::this_thread::sleep_for(50ms);

    u64 elapsed = core::getMonotonicNowNs() - ts;
    u64 stdElapsed = u64(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - stdNow).count());

    u64 diff = elapsed > stdElapsed ? elapsed - stdElapsed : stdElapsed - elapsed;
    // Allow a tolerance of 1 millisecond for elapsed time.
    CT_CHECK(diff < 1000000);

    return 0;
}

i32 performanceCounterTest() {
    using namespace std::chrono_literals;

    u64 beginTsc = core::getPerfCounter();
    auto stdNow = std::chrono::high_resolution_clock::now();

    std::this_thread::sleep_for(50ms);

    u64 stdElapsedNs = u64(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now() - stdNow).count());

    u64 endTsc = core::getPerfCounter();
    u64 freq = core::getCPUFrequencyHz();
    u64 elapsedTsc = endTsc - beginTsc;
    u64 elapsedNs = u64(f64(core::CORE_SECOND) * (f64(elapsedTsc) / f64(freq)));

    u64 diff = elapsedNs > stdElapsedNs ? elapsedNs - stdElapsedNs : stdElapsedNs - elapsedNs;
    // Allow a tolerance of 1 millisecond for elapsed time.
    CT_CHECK(diff < 100000);

    return 0;
}

i32 runPltTimeTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(unixTimestampTest);
    if (runTest(tInfo, unixTimestampTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(monotonicTimestampTest);
    if (runTest(tInfo, monotonicTimestampTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(performanceCounterTest);
    if (runTest(tInfo, performanceCounterTest) != 0) { ret = -1; }

    return ret;
}
