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

i32 os_open_nonexisting_file_test() {
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

template <typename TErr>
void __check_verbose_os_err(const TErr& e) {
    if (e.has_err()) {
        const char* errCptr = core::os_get_err_cptr(e.err());
        Assert(false, errCptr);
    }
}

void __close_testf(core::file_desc fdesc) {
    __check_verbose_os_err(core::os_close(fdesc));
}

void __delete_testf(const char* path) {
    __check_verbose_os_err(core::os_rmfile(path));
}

core::file_desc __open_testf(const char* path, const core::file_params& params) {
    auto res = core::os_open(path, params);
    __check_verbose_os_err(res);
    Assert(res.has_value());
    return res.value();
}

core::file_desc __create_testf(const char* path, const core::file_access_group& access) {
    auto res = core::os_create(path, access);
    __check_verbose_os_err(res);
    Assert(res.has_value());
    return res.value();
}

void __create_testdir(const char* path) {
    __check_verbose_os_err(core::os_mkdir(path));
}

void __delete_testdir(const char* path) {
    __check_verbose_os_err(core::os_rmdir(path));
}

i64 __write_small_testmsg(core::file_desc fdesc, const char* msg, addr_size msgLen) {
    i64 bytesWritten = 0;
    auto res = core::os_write(fdesc, msg, msgLen, bytesWritten);
    __check_verbose_os_err(res);
    Assert(bytesWritten == i64(msgLen));
    return bytesWritten;
}

i64 __read_small_testmsg(core::file_desc fdesc, char* buf, addr_size bufLen) {
    i64 bytesRead = 0;
    auto res = core::os_read(fdesc, buf, bufLen, bytesRead);
    __check_verbose_os_err(res);
    Assert(bytesRead == i64(bufLen));
    return bytesRead;
}

addr_off __seek_to_begging_testf(core::file_desc fdesc) {
    auto res = core::os_seek(fdesc, 0, core::seek_origin::Begin);
    __check_verbose_os_err(res);
    Assert(res.value() == 0);
    return res.value();
}

i32 os_open_with_create_test() {
    core::file_params params = core::default_file_params();
    params.flags = core::file_flags(params.flags | core::FF_Create);

    core::file_desc fdesc = __open_testf(__test_f1_path, params);
    __close_testf(fdesc);
    __delete_testf(__test_f1_path);

    return 0;
}

i32 os_open_with_append_test() {
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

i32 os_open_with_trunc_test() {
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

    // Open the file again with TRUNC.
    params.flags = core::file_flags(params.flags | core::FF_Trunc);
    fdesc = __open_testf(__test_f1_path, params);

    // Write the second message
    __write_small_testmsg(fdesc, worldMsg, worldMsgLen);
    __seek_to_begging_testf(fdesc);

    // Read and verify.. It should have been truncated.
    {
        char buf[worldMsgLen] = {};
        __read_small_testmsg(fdesc, buf, worldMsgLen);
        Assert(core::cptr_eq(buf, worldMsg, worldMsgLen));
    }

    __close_testf(fdesc);

    __delete_testf(__test_f1_path);

    return 0;
}

i32 os_walk_directories_test() {

    __create_testdir(PATH_TO_TEST_DATA"/d1");
    __create_testdir(PATH_TO_TEST_DATA"/d1/d2");
    __create_testdir(PATH_TO_TEST_DATA"/d3");
    auto f2 = __create_testf(PATH_TO_TEST_DATA"/d1/f2.txt", core::default_file_access_group());
    auto f1 = __create_testf(__test_f1_path, core::default_file_access_group());

    auto d1 = ValueOrDie(core::os_opendir(PATH_TO_TEST_DATA"/d1"));
    auto d3 = ValueOrDie(core::os_opendir(PATH_TO_TEST_DATA"/d3"));

    auto d1stat = ValueOrDie(core::os_fstat(d1));
    Assert(d1stat.isDir());

    auto d2stat = ValueOrDie(core::os_stat(PATH_TO_TEST_DATA"/d1/d2"));
    Assert(d2stat.isDir());

    auto d3stat = ValueOrDie(core::os_fstat(d3));
    Assert(d3stat.isDir());

    auto f2stat = ValueOrDie(core::os_fstat(f2));
    Assert(!f2stat.isDir());

    // Walk the test data directory:
    {
        const char* rootDir = PATH_TO_TEST_DATA;
        i32 counter = 0;
        struct dirEntry {
            core::file_type type;
            char name[core::MAX_FILE_LENGTH];
        };
        dirEntry rootEntries[3] = {};
        core::os_dir_walk(rootDir, [&](const core::dir_entry& de) -> bool {
            dirEntry& entry = rootEntries[counter++];
            entry.type = de.type;
            core::cptr_copy(entry.name, de.name, core::cptr_len(de.name));
            return true;
        });

        Assert(counter == 3);

        // The order in which entries are returned might be different on different platforms.
        // So search the entire entries array for the expected entries:

        addr_off idx = 0;
        idx = core::find(rootEntries, 3, [](auto& elem, addr_off) {
            bool ret = core::cptr_eq(elem.name, "f1.txt", 6);
            ret &= elem.type == core::file_type::Regular;
            return ret;
        });
        Assert(idx != -1);

        idx = core::find(rootEntries, 3, [](auto& elem, addr_off) {
            bool ret = core::cptr_eq(elem.name, "d1", 2);
            ret &= elem.type == core::file_type::Directory;
            return ret;
        });
        Assert(idx != -1);

        idx = core::find(rootEntries, 3, [](auto& elem, addr_off) {
            bool ret = core::cptr_eq(elem.name, "d3", 2);
            ret &= elem.type == core::file_type::Directory;
            return ret;
        });
        Assert(idx != -1);
    }

    __close_testf(f1);
    __close_testf(f2);
    __close_testf(d1);
    __close_testf(d3);

    __delete_testf(PATH_TO_TEST_DATA"/f1.txt");
    __delete_testf(PATH_TO_TEST_DATA"/d1/f2.txt");
    __delete_testdir(PATH_TO_TEST_DATA"/d1/d2");
    __delete_testdir(PATH_TO_TEST_DATA"/d1");
    __delete_testdir(PATH_TO_TEST_DATA"/d3");

    return 0;
}

template <typename TAllocator>
i32 os_fs_read_write_test() {
    core::file wf;
    {
        auto params = core::default_file_params();
        params.flags = core::file_flags(core::FF_Create | core::FF_WriteOnly | core::FF_Trunc);
        auto res = core::os_open(__test_f1_path, params);
        Assert(!res.has_err());
        Assert(res.has_value());
        Expect(wf.take_desc(core::move(res.value())));
        Assert(!wf.is_directory());
    }
    defer { Expect(wf.close()); };

    core::file rf;
    {
        auto params = core::default_file_params();
        params.flags = core::file_flags(core::FF_ReadOnly);
        auto res = core::os_open(__test_f1_path, params);
        Assert(!res.has_err());
        Assert(res.has_value());
        Expect(rf.take_desc(core::move(res.value())));
        Assert(!rf.is_directory());
    }
    defer { Expect(wf.close()); };

    {
        constexpr const char* msg = "aaa";
        addr_size written = ValueOrDie(wf.write(msg, core::cptr_len(msg)));
        Assert(written == core::cptr_len(msg));

        char buf[core::cptr_len(msg)] = {};
        addr_size read = ValueOrDie(rf.read(buf, core::cptr_len(msg)));
        Assert(read == core::cptr_len(msg));
        Assert(core::cptr_eq(buf, msg, core::cptr_len(msg)));
    }

    {
        constexpr const char* msg = "bbb";
        addr_size written = ValueOrDie(wf.write(msg, core::cptr_len(msg)));
        Assert(written == core::cptr_len(msg));

        char buf[core::cptr_len(msg)] = {};
        addr_size read = ValueOrDie(rf.read(buf, core::cptr_len(msg)));
        Assert(read == core::cptr_len(msg));
        Assert(core::cptr_eq(buf, msg, core::cptr_len(msg)));
    }

    // Seek from begining:
    {
        Expect(wf.seek(2, core::seek_origin::Begin));

        constexpr const char* msg = "ccc";
        addr_size written = ValueOrDie(wf.write(msg, core::cptr_len(msg)));
        Assert(written == core::cptr_len(msg));

        core::arr<char, TAllocator> out;
        Expect(core::file_read_full(__test_f1_path, out));
        Assert(core::cptr_eq(out.data(), "aacccb", out.len()));
    }

    // Seek from current:
    {
        Expect(wf.seek(1, core::seek_origin::Current));

        constexpr const char* msg = "dd";
        addr_size written = ValueOrDie(wf.write(msg, core::cptr_len(msg)));
        Assert(written == core::cptr_len(msg));

        core::arr<char, TAllocator> out;
        Expect(core::file_read_full(__test_f1_path, out));
        Assert(core::cptr_eq(out.data(), "aacccbdd", out.len()));
    }

    // Seek from end with negative offset.
    {
        Expect(wf.seek(-3, core::seek_origin::End));

        constexpr const char* msg = "ppp";
        addr_size written = ValueOrDie(wf.write(msg, core::cptr_len(msg)));
        Assert(written == core::cptr_len(msg));

        core::arr<char, TAllocator> out;
        Expect(core::file_read_full(__test_f1_path, out));
        Assert(core::cptr_eq(out.data(), "aacccppp", out.len()));
    }

    // Write more than 2 times the block size of data and read it back.
    {
        Expect(wf.seek(0, core::seek_origin::Begin));

        const addr_size bufLen = core::os_get_default_block_size() * 2 + 100;
        char buf[bufLen];
        core::memset(buf, '9', bufLen); // all 9s
        Expect(core::write_full(wf, buf, bufLen));

        core::arr<char, TAllocator> out;
        Expect(core::file_read_full(__test_f1_path, out));
        Assert(out.len() == bufLen);
        for (addr_size i = 0; i < bufLen; ++i) {
            Assert(out[i] == '9'); // all 9s baby
        }
    }

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

    RunTest(os_open_nonexisting_file_test);
    RunTest(os_open_with_create_test);
    RunTest(os_open_with_append_test);
    RunTest(os_open_with_trunc_test);
    RunTest(os_walk_directories_test);

    RunTest(os_fs_read_write_test<std_allocator_static>);

    return 0;
}
