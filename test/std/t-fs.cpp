#include <fcntl.h>

void basic_test() {
    using File = core::File<core::FS_DEFAULT_BLOCK_SIZE>;
    constexpr const char* pathToTestFile = PATH_TO_TEST_DATA "/basic_test.txt";

    File f1;
    defer {
        Check(f1.close());
        Assert(!f1.isOpen());
    };
    {
        auto res = core::openfile(pathToTestFile, O_CREAT | O_RDWR | O_TRUNC, 0666);
        f1 = ValueOrDie(res);
        Assert(f1.isOpen());
    }
    {
        constexpr std::string_view phrase = "Hello World!";
        ptr_size written = 0;
        auto res = f1.write(phrase.data(), phrase.size(), written);
        Assert(!res.has_err());
        Assert(written == phrase.size());
    }

    File f2;
    defer {
        Check(f2.close());
        Assert(!f2.isOpen());
    };
    {
        auto res = core::openfile(pathToTestFile, O_RDONLY, 0666);
        f2 = ValueOrDie(res);
        Assert(f2.isOpen());
    }
    {
        constexpr std::string_view phrase = "Hello World!";
        char data[500] = {};
        ptr_size readBytes = 0;
        auto res = f2.read(data, 500, readBytes);
        Assert(res.has_err() && res.err().isEOF(), "Read should have returned EOF error!");
        Assert(readBytes == phrase.size());
        Assert(core::cptr_cmp(data, phrase.data()) == 0);
    }

    Assert(!core::rmfile(f1).has_err());
}

void readfull_test() {
    {
        constexpr const char* pathToTestFile = PATH_TO_TEST_DATA "/readfull_test.txt";

        core::File<core::FS_DEFAULT_BLOCK_SIZE> f1;
        {
            auto res = core::openfile(pathToTestFile, O_CREAT | O_RDWR | O_TRUNC, 0666);
            f1 = ValueOrDie(res);
            Assert(f1.isOpen());
        }
        constexpr std::string_view phrase = "123456789";
        {
            ptr_size written = 0;
            auto res = f1.write(phrase.data(), phrase.size(), written);
            Assert(!res.has_err());
            Assert(written == phrase.size());
        }
        Check(f1.close());
        Assert(!f1.isOpen());

        auto res = core::readfull<3, std_allocator_static>(pathToTestFile, O_RDONLY, 0666);
        Assert(!res.has_err());
        core::arr<u8, std_allocator_static>& data = res.value();
        Assert(data.len() == phrase.size());
        const char* d = reinterpret_cast<const char*>(data.data());
        Assert(core::cptr_cmp(d, phrase.data()) == 0);
    }

    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
}

void run_fs_tests_suite() {
#if (defined(OS_WIN) && OS_WIN == 0) // FIXME: reintroduce test and write real tests when the windows api is ready

    bool exists = ValueOrDie(core::os_exists(PATH_TO_TEST_DATA));
    if (!exists) {
        Check(core::os_mkdir(PATH_TO_TEST_DATA, 0777));
    }

    RunTest(basic_test);
    RunTest(readfull_test);
#endif
}
