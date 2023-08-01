i32 get_time_test() {
    auto res = core::os_unix_time_stamp_in_ms();
    Assert(res.has_value());
    Assert(!res.has_err());
    Assert(res.value() > 0);

    return 0;
}

i32 thread_sleep_test() {
    Expect(core::os_thread_sleep(100));

    return 0;
}

i32 os_alloc_de_alloc_pages_test() {
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
    }
    {
        auto deallocErr = core::os_dealloc_pages(res.value(), 0);
        Assert(deallocErr.has_err(), "deallocation of size 0 should fail");
    }
    {
        auto deallocErr = core::os_dealloc_pages(nullptr, 1024);
        Assert(deallocErr.has_err());
        Assert(deallocErr.err() == core::OS_DEALLOC_NULL_ADDR_ERR, "error code equality check should work bi-directionally");
    }

    return 0;
}

i32 run_plt_tests_suite() {
    RunTest(get_time_test);
    RunTest(thread_sleep_test);
    RunTest(os_alloc_de_alloc_pages_test);

    return 0;
}
