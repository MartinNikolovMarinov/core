#include "tests/t-index.h"

#include <iostream>
#include <unordered_map>
#include <ctime>
#include <cstdlib>
#include <string>

i32 main() {
    core::initProgramCtx(nullptr, nullptr);

    core::Thread t1;
    core::Expect(core::threadInit(t1));

    i32 arg = 42;

    auto startErr = core::threadStart(t1, reinterpret_cast<void*>(&arg), [](void* _arg) {
        core::threadingSleep(5000);
        i32* passedArg = reinterpret_cast<i32*>(_arg);
        Panic(passedArg, "The passed argument is nullptr!");
        *passedArg += 10;
        std::cout << "Arg in worker is " << *passedArg << std::endl;
    });
    core::Expect(std::move(startErr));

    std::cout << "Spinny spinny..." << std::endl;
    while (!t1.isRunning) {
        core::threadingSleep(500);
        std::cout << "Waiting for thread 1 to start..." << std::endl;
    }

    core::Expect(core::threadJoin(t1));

    std::cout << "Arg in main thread is " << arg << std::endl;

    return 0;
}
