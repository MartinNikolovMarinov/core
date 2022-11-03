#include <plt.h>

void GetTimeTest() {
    auto res = core::plt::OsUnixTimeStampInMs();
    Assert(!res.err.IsErr());
    Assert(res.val > 0);
}

void ThreadSleepTest() {
    core::plt::OsThreadSleep(100);
}

void OsAllocDeAllocPagesTest() {
    auto res = core::plt::OsAllocPages(1024);
    Assert(!res.err.IsErr());
    Assert(res.val != nullptr);

    auto resLarge = core::plt::OsAllocPages(core::TERABYTE * 10000);
    Assert(resLarge.err.IsErr(), "should fail when allcation is too large");
    Assert(resLarge.val == nullptr, "should return nullptr when allocation fails");

    auto deallocErr = core::plt::OsDeallocPages(res.val, 1024);
    Assert(!deallocErr.IsErr());
    deallocErr = core::plt::OsDeallocPages(res.val, 0);
    Assert(deallocErr.IsErr(), "deallocation of size 0 should fail");
    deallocErr = core::plt::OsDeallocPages(nullptr, 1024);
    Assert(core::plt::OsDeallocNullAddrErr == deallocErr.Err(), "deallocation of nullptr should fail");
    Assert(deallocErr.Err() == core::plt::OsDeallocNullAddrErr, "error code equality check should work bi-directionally");
}

void RunPltTestSuite() {
    RunTest(GetTimeTest);
    RunTest(ThreadSleepTest);
    RunTest(OsAllocDeAllocPagesTest);
}
