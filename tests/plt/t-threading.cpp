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

i32 runPltThreadingTestsSuite() {
    RunTest(getNumberOfCoresTest);
    return 0;
}
