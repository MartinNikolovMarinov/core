#include <fcntl.h>

// FIXME: Allow the user to use a file path in the arguments for this tests ?
//        Skip the test suite if that file is not found.

void TMP_test() {
    using file = core::file<core::FS_DEFAULT_BLOCK_SIZE>;

    file f;
    // defer { Check(f2.close()); }; // this will fail because f2 might have already deleted the file.
    {
        auto res = file::open_file("example_dump_file.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
        f = ValueOrDie(res);
        Assert(f.is_open());
    }
    constexpr std::string_view phrase = "Hello World!";
    {
        auto res = f.write(phrase.data(), phrase.size());
        Assert(!res.has_err());
        ptr_size writtenBytes = res.value();
        Assert(writtenBytes == phrase.size());
    }

    file f2;
    defer { Check(f2.close()); };
    {
        Assert(!f2.open("example_dump_file.txt", O_RDONLY, 0666).has_err());
        Assert(f2.is_open());
    }
    {
        char data[500] = {};
        auto res = f2.read(data, 500);
        Assert(!res.has_err() || res.is_eof());
        ptr_size readBytes = res.value();
        Assert(readBytes == phrase.size());
        Assert(core::cptr_cmp(data, phrase.data()) == 0);
    }

    Assert(!file::rmfile(f2).has_err());
}

void run_fs_tests_suite() {
    RunTest(TMP_test);
}
