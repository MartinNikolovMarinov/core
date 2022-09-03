void GetTimeTest() {
    auto res = core::plt::UnixTimeStampInMs();
    Assert(res.b.isOk());
    Assert(res.a > 0);
}

void ThreadSleepTest() {
    core::plt::ThreadSleep(100);
}

void OsAllocDeAllocPagesTest() {
    auto res = core::plt::OsAllocPages(1024);
    Assert(res.b.isOk());
    Assert(res.a != nullptr);

    auto resLarge = core::plt::OsAllocPages(core::TERABYTE * 10000);
    Assert(!resLarge.b.isOk(), "should fail when allcation is too large");
    Assert(resLarge.a == nullptr, "should return nullptr when allocation fails");

    auto deallocErr = core::plt::OsDeallocPages(res.a, 1024);
    Assert(deallocErr.isOk());
    deallocErr = core::plt::OsDeallocPages(res.a, 0);
    Assert(!deallocErr.isOk(), "deallocation of size 0 should fail");
    deallocErr = core::plt::OsDeallocPages(nullptr, 1024);
    Assert(core::plt::PltErr::DeallocNullAddr == deallocErr.code, "deallocation of nullptr should fail");
    Assert(deallocErr.code == core::plt::PltErr::DeallocNullAddr, "error code equality check should work bi-directionally");
}

void RunPltTestSuite() {
    RunTest(GetTimeTest);
    RunTest(ThreadSleepTest);
    RunTest(OsAllocDeAllocPagesTest);
}
