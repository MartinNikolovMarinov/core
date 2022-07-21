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

void WriteLine(const char* data) {
    u64 ret = OS_write(1, data, core::CptrLen(data));
    if (ret <= 0) OS_exit(1);
}

#define RunTest(test)                             \
    WriteLine("\t[TEST RUNNING] ");               \
    WriteLine(#test);                             \
    WriteLine("\n");                              \
    test();                                       \
    WriteLine("\t[TEST \x1b[32mPASSED\x1b[0m] "); \
    WriteLine(#test);                             \
    WriteLine("\n");

#define RunTestSuite(suite)                      \
    WriteLine("[SUITE RUNNING] ");               \
    WriteLine(#suite);                           \
    WriteLine("\n");                             \
    suite();                                     \
    WriteLine("[SUITE \x1b[32mPASSED\x1b[0m] "); \
    WriteLine(#suite);                           \
    WriteLine("\n");

template<typename T>
T CheckTuple(core::Tuple<T, bool> t, const char* msg = "") {
    Assert(t.b, msg);
    return t.a;
}

#include "test/run_tests.cpp"

i32 main(i32, const char**, const char **) {
    core::SetGlobalAssertHandler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        WriteLine("[ASSERTION] [EXPR]: ");
        WriteLine(failedExpr);
        WriteLine(" [FILE]: ");
        WriteLine(file);
        WriteLine(" [LINE]: ");
        char lineBuf[50];
        core::MemSet(lineBuf, 0, 50);
        core::IntToCptr<i32>(line, lineBuf);
        WriteLine(lineBuf);
        WriteLine(" [MSG]: ");
        WriteLine(errMsg);
        WriteLine("\n");
        return false;
    });

    if (COMPILER_CLANG == 1)   { WriteLine("[COMPILER] COMPILER_CLANG\n"); }
    if (COMPILER_GCC == 1)     { WriteLine("[COMPILER] COMPILER_GCC\n"); }
    if (COMPILER_MSVC == 1)    { WriteLine("[COMPILER] COMPILER_MSVC\n"); }
    if (COMPILER_UNKNOWN == 1) { WriteLine("[COMPILER] COMPILER_UNKNOWN\n"); }

    if (OS_WIN == 1)     { WriteLine("[OS] OS_WIN\n"); }
    if (OS_LINUX == 1)   { WriteLine("[OS] OS_LINUX\n"); }
    if (OS_UNIX == 1)    { WriteLine("[OS] OS_UNIX\n"); }
    if (OS_UNKNOWN == 1) { WriteLine("[OS] OS_UNKNOWN\n"); }

    RunAllTests();

    WriteLine("\n");
    WriteLine("\x1b[32m\x1b[1mTests OK\x1b[0m\n");
    return 0;
}
