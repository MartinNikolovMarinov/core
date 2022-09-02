void GetTimeTest() {
    auto res = plt::UnixTimeStampInMs();
    Assert(res.b.isOk());
    Assert(res.a > 0);
}

void ThreadSleepTest() {
    plt::ThreadSleep(100);
}

void OsAllocDeAllocPagesTest() {
    auto res = plt::OsAllocPages(1024);
    Assert(res.b.isOk());
    Assert(res.a != nullptr);

    auto resLarge = plt::OsAllocPages(core::TERABYTE * 10000);
    Assert(!resLarge.b.isOk(), "should fail when allcation is too large");
    Assert(resLarge.a == nullptr, "should return nullptr when allocation fails");

    auto deallocErr = plt::OsDeallocPages(res.a, 1024);
    Assert(deallocErr.isOk());
    deallocErr = plt::OsDeallocPages(res.a, 0);
    Assert(!deallocErr.isOk(), "deallocation of size 0 should fail");
    deallocErr = plt::OsDeallocPages(nullptr, 1024);
    Assert(plt::PltErr::DeallocNullAddr == deallocErr.code, "deallocation of nullptr should fail");
    Assert(deallocErr.code == plt::PltErr::DeallocNullAddr, "error code equality check should work bi-directionally");
}

void RunPltTestSuite() {
    RunTest(GetTimeTest);
    RunTest(ThreadSleepTest);
    RunTest(OsAllocDeAllocPagesTest);
}
