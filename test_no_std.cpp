#include "src/core.h"
#include <sys/syscall.h> // incuded for system call numbers

// NOTE: unistd.h cannot be used because it requires the standard library.
// #include <unistd.h>

using namespace coretypes;

extern "C" {
    extern u64 syscall (u64 __sysno, ...);
}

void OS_exit(i32 _code) {
    syscall(SYS_exit_group, reinterpret_cast<void*>(_code), 0, 0, 0, 0);
    for (;;) syscall(SYS_exit, reinterpret_cast<void*>(_code), 0, 0, 0, 0);
}

u64 OS_write(i64 fd, const char* data, u64 nbytes) {
    u64 res = (u64)syscall(SYS_write, fd, data, nbytes, 0, 0);
    return res;
}

void write_line(const char* data) {
    u64 ret = OS_write(1, data, core::cptr_len(data));
    if (ret <= 0) OS_exit(1);
}

#define RunTest(test, ...)                                       \
    write_line("\t[TEST RUNNING] ");                             \
    write_line(ANSI_BOLD(#test));                                \
    write_line("\n");                                            \
    test(__VA_ARGS__);                                           \
    write_line("\t[TEST " ANSI_BOLD(ANSI_GREEN("PASSED")) "] "); \
    write_line(ANSI_BOLD(#test));                                \
    write_line("\n");

#define RunTestCompileTime(test, ...)                                                           \
    { [[maybe_unused]] constexpr auto __notused__ = core::force_consteval<test(__VA_ARGS__)>; } \

#define RunTestSuite(suite, ...)                                \
    write_line("[SUITE RUNNING] ");                             \
    write_line(ANSI_BOLD(#suite));                              \
    write_line("\n");                                           \
    suite(__VA_ARGS__);                                         \
    write_line("[SUITE " ANSI_BOLD(ANSI_GREEN("PASSED")) "] "); \
    write_line(ANSI_BOLD(#suite));                              \
    write_line("\n");

#ifndef CORE_RUN_COMPILETIME_TESTS
    #define CORE_RUN_COMPILETIME_TESTS 0
#endif

#include "common_test_helpers.h"
#include "test/run_tests.cpp"

i32 main(i32, const char**, const char**) {
     core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        write_line(ANSI_RED_START());
        write_line(ANSI_BOLD_START());
        write_line("[ASSERTION] [EXPR]: ");
        write_line(failedExpr);
        write_line(" [FILE]: ");
        write_line(file);
        write_line(" [LINE]: ");
        char lineBuf[50];
        core::memset(lineBuf, 0, 50);
        core::int_to_cptr<i32>(line, lineBuf);
        write_line(lineBuf);
        write_line(" [MSG]: ");
        write_line(errMsg);
        write_line("\n");
        write_line(ANSI_RESET());
        return true;
    });

    if constexpr (COMPILER_CLANG == 1)   { write_line("[COMPILER] COMPILER_CLANG\n"); }
    if constexpr (COMPILER_GCC == 1)     { write_line("[COMPILER] COMPILER_GCC\n"); }
    if constexpr (COMPILER_MSVC == 1)    { write_line("[COMPILER] COMPILER_MSVC\n"); }
    if constexpr (COMPILER_UNKNOWN == 1) { write_line("[COMPILER] COMPILER_UNKNOWN\n"); }

    if constexpr (OS_WIN == 1)     { write_line("[OS] OS_WIN\n"); }
    if constexpr (OS_LINUX == 1)   { write_line("[OS] OS_LINUX\n"); }
    if constexpr (OS_MAC == 1)     { write_line("[OS] OS_MAC\n"); }
    if constexpr (OS_UNKNOWN == 1) { write_line("[OS] OS_UNKNOWN\n"); }

    write_line("\nRUNNING COMMON TESTS\n\n");
    i32 exitCode = run_all_tests();

    write_line("\n");
    write_line(ANSI_BOLD(ANSI_GREEN("Tests OK")));
    write_line("\n");

    if constexpr (CORE_RUN_COMPILETIME_TESTS != 1) {
        write_line(ANSI_YELLOW_START());
        write_line(ANSI_BOLD_START());
        write_line("[WARN] DID NOT RUN COMPILETIME TESTS!");
        write_line(ANSI_RESET());
    }

    return exitCode;
}
