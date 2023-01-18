#include <core.h>
#include <std/core.h>
#include <iostream>

using namespace coretypes;

#define RunTest(test, ...)                                                  \
    std::cout << "\t[TEST RUNNING] " << #test << '\n';                      \
    test(__VA_ARGS__);                                                      \
    std::cout << "\t[TEST \x1b[32mPASSED\x1b[0m] " << #test << std::endl;

#define RunTestSuite(suite, ...)                                            \
    std::cout << "[SUITE RUNNING] " << #suite << std::endl;                 \
    suite(__VA_ARGS__);                                                     \
    std::cout << "[SUITE \x1b[32mPASSED\x1b[0m] " << #suite << std::endl;

#include "test/run_tests.cpp"
#include "test/std/run_tests.cpp"

#include <fcntl.h>

// FIXME: write this in a proper test.
// FIXME: Test with empty files. There seems to be some issue that.
void tmp_file_test() {
    using file = core::file<std_allocator_static>;

    file f;
    // defer { Check(f2.close()); }; // this will fail because f2 might have already deleted the file.
    {
        auto res = file::open_file("example_dump_file.txt",  O_CREAT | O_RDWR | O_TRUNC, 0666);
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

i32 main(i32, const char **) {
    core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        std::cout << "[ASSERTION] [EXPR]: " << failedExpr
                << " [FILE]: " << file
                << " [LINE]: " << line
                << " [MSG]: " << errMsg
                << std::endl; // flush stream!
        throw std::runtime_error("Assertion failed!");
        return false;
    });

    // Print compiler
    if (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
    if (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
    if (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
    if (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

    // Print OS
    if (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
    if (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
    if (OS_UNIX == 1)    { std::cout << "[OS] OS_UNIX" << std::endl; }
    if (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

    std::cout << "\n" << "RUNNING COMMONG TESTS" << "\n\n";
    run_all_tests();
    std::cout << "\n" << "RUNNING STD TESTS" << "\n\n";
    run_all_std_tests();

    // FIXME: remove
    tmp_file_test();

    std::cout << '\n';
    std::cout << "\x1b[32m\x1b[1mTests OK\x1b[0m\n";
    return 0;
}
