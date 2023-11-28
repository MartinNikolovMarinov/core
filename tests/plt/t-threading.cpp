#include "../t-index.h"

#include <thread>
#include <iostream>

i32 getNumberOfCoresTest() {
    i32 n = ValueOrDie(core::threadingGetNumCores());
    Assert(n > 0);
    u32 stdN = std::thread::hardware_concurrency();
    Assert(u32(n) == stdN);
    return 0;
}

i32 start2ThreadsAndJoinThemTest() {
    core::Thread t1;
    core::Thread t2;

    Expect(core::threadInit(t1));
    Expect(core::threadInit(t2));

    Assert(core::threadIsRunning(t1) == false);
    Assert(core::threadIsRunning(t2) == false);

    core::threadStart(t1, &t1, [](void* arg) -> void* {
        core::Thread *t = reinterpret_cast<core::Thread*>(arg);
        Assert(core::threadIsRunning(*t) == true); // Don't try this at home!
        return nullptr;
    });

    core::threadStart(t2, &t2, [](void* arg) -> void* {
        core::Thread *t = reinterpret_cast<core::Thread*>(arg);
        Assert(core::threadIsRunning(*t) == true);
        return nullptr;
    });

    Expect(core::threadJoin(t1));
    Assert(core::threadIsRunning(t1) == false);
    Expect(core::threadJoin(t2));
    Assert(core::threadIsRunning(t2) == false);

    return 0;
}

i32 runPltThreadingTestsSuite() {
    RunTest(getNumberOfCoresTest);
    RunTest(start2ThreadsAndJoinThemTest);

    return 0;
}
