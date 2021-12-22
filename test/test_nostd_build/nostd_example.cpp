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

i32 main(i32 _argc, const char** _argv, const char **_envp) {
    core::Error e("t");
    auto err = e.Err();
    e.Reset("tt");
    err = e.Err();
    e.Reset("ttt");
    err = e.Err();
    e.Reset("tttt");
    err = e.Err();
    e.Reset("ttttt");
    err = e.Err();
    e.Reset("tttttt");
    err = e.Err();
    e.Reset("ttttttt");
    err = e.Err();
    e.Reset("tttttttt");
    err = e.Err();
    e.Reset("ttttttttt");
    err = e.Err();
    e.Reset("tttttttttt");
    err = e.Err();
    OS_exit(12);
    return 0;
}
