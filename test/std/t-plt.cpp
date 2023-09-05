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

i32 os_open_nonexisting_file() {
    core::file_params params = {};
    params.access = core::default_file_access_group();
    params.flags = core::FF_None;

    {
        auto res = core::os_open("/zxcasd1sd", params);
        Assert(!res.has_value());
        Assert(res.has_err());
        const char* errCptr = core::os_get_err_cptr(res.err());
        Assert(errCptr != nullptr);
    }

    return 0;
}

constexpr const char* __test_f1_path = PATH_TO_TEST_DATA"/f1.txt";

void __close_testf(core::file_desc fdesc) {
    auto res = core::os_close(fdesc);
    Assert(!res.has_err());
}

void __delete_testf(const char* path) {
    auto res = core::os_rmfile(path);
    Assert(!res.has_err());
}

core::file_desc __open_testf(const char* path, const core::file_params& params) {
    auto res = core::os_open(path, params);
    Assert(res.has_value());
    Assert(!res.has_err());
    core::file_desc fdesc = res.value();
    return fdesc;
}

i64 __write_small_testmsg(core::file_desc fdesc, const char* msg, i32 msgLen) {
    i64 bytesWritten = 0;
    auto res = core::os_write(fdesc, msg, msgLen, bytesWritten);
    Assert(!res.has_err());
    Assert(bytesWritten == msgLen);
    return bytesWritten;
}

i64 __read_small_testmsg(core::file_desc fdesc, char* buf, i32 bufLen) {
    i64 bytesRead = 0;
    auto res = core::os_read(fdesc, buf, bufLen, bytesRead);
    Assert(!res.has_err());
    Assert(bytesRead == bufLen);
    return bytesRead;
}

addr_off __seek_to_begging_testf(core::file_desc fdesc) {
    auto res = core::os_seek(fdesc, 0, core::seek_origin::Begin);
    Assert(!res.has_err());
    Assert(res.value() == 0);
    addr_off ret = res.value();
    return ret;
}

i32 os_open_with_create() {
    core::file_params params = core::default_file_params();
    params.flags = core::file_flags(params.flags | core::FF_Create);

    core::file_desc fdesc = __open_testf(__test_f1_path, params);
    __close_testf(fdesc);
    __delete_testf(__test_f1_path);

    return 0;
}

i32 os_open_with_append() {
    core::file_params params = core::default_file_params();
    params.flags = core::file_flags(params.flags | core::FF_Create);

    core::file_desc fdesc = __open_testf(__test_f1_path, params);

    constexpr const char* helloMsg = "Hello ";
    constexpr i32 helloMsgLen = core::cptr_len(helloMsg);
    constexpr const char* worldMsg = "World!\n";
    constexpr i32 worldMsgLen = core::cptr_len(worldMsg);

    // Write first message.
    __write_small_testmsg(fdesc, helloMsg, helloMsgLen);
    __seek_to_begging_testf(fdesc);

    // Read and verify.
    {
        char buf[helloMsgLen] = {};
        __read_small_testmsg(fdesc, buf, helloMsgLen);
        Assert(core::cptr_eq(buf, helloMsg, helloMsgLen));
    }

    // Close the file
    __close_testf(fdesc);

    // Open the file again with APPEND.
    params.flags = core::file_flags(params.flags | core::FF_Append);
    fdesc = __open_testf(__test_f1_path, params);

    // Write the second message
    __write_small_testmsg(fdesc, worldMsg, worldMsgLen);
    __seek_to_begging_testf(fdesc);

    // Read and verify.. It should have been appended.
    {
        char buf[helloMsgLen + worldMsgLen] = {};
        __read_small_testmsg(fdesc, buf, helloMsgLen + worldMsgLen);
        Assert(core::cptr_eq(buf, "Hello World!\n", helloMsgLen + worldMsgLen));
    }

    __close_testf(fdesc);

    __delete_testf(__test_f1_path);

    return 0;
}

i32 run_plt_tests_suite() {
    RunTest(get_time_test);
    RunTest(thread_sleep_test);
    RunTest(os_alloc_de_alloc_pages_test);

    // Before starting the test suite, make sure the test data directory exists.
    bool exists = ValueOrDie(core::os_exists(PATH_TO_TEST_DATA));
    if (!exists) {
        Expect(core::os_mkdir(PATH_TO_TEST_DATA));
    }

    RunTest(os_open_nonexisting_file);
    RunTest(os_open_with_create);
    RunTest(os_open_with_append);

    return 0;
}
