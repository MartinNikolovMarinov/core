#include "../../src/core.h"

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

void testingDefer(i32& a) {
    defer { a++; };
}

i32 main(i32 _argc, const char** _argv, const char **_envp) {
    i32 x = 12;
    testingDefer(x);

    OS_exit(x);
    return 0;
}
