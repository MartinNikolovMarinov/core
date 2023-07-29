#include <fcntl.h>
#include <string_view>

i32 basic_fs_open_test() {
    constexpr const char* pathToTestFile = PATH_TO_TEST_DATA "/basic_test.txt";

    core::file_data f1;
    defer {
        Check(core::file_close(f1));
        Assert(!f1.isOpen);
    };
    {
        auto res = core::file_open(pathToTestFile, O_CREAT | O_RDWR | O_TRUNC, 0666);
        f1 = ValueOrDie(res);
        Assert(f1.isOpen);
    }
    defer {
        Check(core::os_rmfile(pathToTestFile));
    };
    {
        constexpr std::string_view phrase = "Hello World!";
        ptr_size written = 0;
        auto res = core::file_write(f1, phrase.data(), phrase.size(), written);
        Assert(!res.has_err());
        Assert(written == phrase.size());
    }

    core::file_data f2;
    defer {
        Check(core::file_close(f2));
        Assert(!f2.isOpen);
    };
    {
        auto res = core::file_open(pathToTestFile, O_RDONLY, 0666);
        f2 = ValueOrDie(res);
        Assert(f2.isOpen);
    }
    {
        constexpr std::string_view phrase = "Hello World!";
        char data[500] = {};
        ptr_size readBytes = 0;
        auto res = core::file_read(f2, data, 500, readBytes);
        Assert(res.has_err() && res.err().is_eof(), "Read should have returned EOF error!");
        Assert(readBytes == phrase.size());
        Assert(core::cptr_cmp(data, core::cptr_len(data), phrase.data(), phrase.length()) == 0);
    }

    return 0;
}

i32 read_full_fs_test() {
    {
        constexpr const char* pathToTestFile = PATH_TO_TEST_DATA "/readfull_test.txt";

        core::file_data f1;
        {
            auto res = core::file_open(pathToTestFile, O_CREAT | O_RDWR | O_TRUNC, 0666);
            f1 = ValueOrDie(res);
            Assert(f1.isOpen);
        }
        defer {
            Check(core::os_rmfile(pathToTestFile));
            Check(core::file_close(f1));
            Assert(!f1.isOpen);
        };
        constexpr std::string_view phrase = "123456789";
        {
            ptr_size written = 0;
            auto res = core::file_write(f1, phrase.data(), phrase.size(), written);
            Assert(!res.has_err());
            Assert(written == phrase.size());
        }

        auto res = core::file_read_full<std_allocator_static>(pathToTestFile, O_RDONLY, 0666);
        Assert(!res.has_err());
        core::arr<u8, std_allocator_static> data = core::move(res.value());
        Assert(data.len() == phrase.size());
        const char* ccdata = reinterpret_cast<const char*>(data.data());
        Assert(core::cptr_cmp(ccdata, data.len(), phrase.data(), phrase.length()) == 0);
    }

    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");

    return 0;
}

i32 run_fs_tests_suite() {
    // Before starting the test suite, make sure the test data directory exists.
    bool exists = ValueOrDie(core::os_exists(PATH_TO_TEST_DATA));
    if (!exists) {
        Check(core::os_mkdir(PATH_TO_TEST_DATA, 0777));
    }

    RunTest(basic_fs_open_test);
    RunTest(read_full_fs_test);

    return 0;
}
