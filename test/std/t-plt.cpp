void get_time_test() {
    auto res = core::os_unix_time_stamp_in_ms();
    Assert(res.has_value());
    Assert(!res.has_err());
    Assert(res.value() > 0);
}

void thread_sleep_test() {
    core::os_thread_sleep(100); // don't explode I guess...
}

void os_alloc_de_alloc_pages_test() {
    auto res = core::os_alloc_pages(1024);
    Assert(res.has_value());
    Assert(!res.has_err());
    Assert(res.value() != nullptr);

    auto resLarge = core::os_alloc_pages(core::TERABYTE * 10000);
    Assert(resLarge.has_err());
    Assert(!resLarge.has_value());

    {
        auto deallocErr = core::os_dealloc_pages(res.value(), 1024);
        Assert(!deallocErr.has_err());
        Assert(deallocErr.has_value());
        Assert(deallocErr.value() == 0);
    }
    {
        auto deallocErr = core::os_dealloc_pages(res.value(), 0);
        Assert(deallocErr.has_err(), "deallocation of size 0 should fail");
        Assert(!deallocErr.has_value());
    }
    {
        auto deallocErr = core::os_dealloc_pages(nullptr, 1024);
        Assert(deallocErr.has_err());
        Assert(!deallocErr.has_value());
        Assert(deallocErr.err() == core::OS_DEALLOC_NULL_ADDR_ERR, "error code equality check should work bi-directionally");
    }
}

void run_plt_tests_suite() {
    RunTest(get_time_test);
    RunTest(thread_sleep_test);
    RunTest(os_alloc_de_alloc_pages_test);
}
