#include "../t-index.h"

#include <thread>

i32 getNumberOfCoresTest() {
    i32 n = core::Unpack(core::threadingGetNumCores());
    CT_CHECK(n > 0);
    u32 stdN = std::thread::hardware_concurrency();
    CT_CHECK(u32(n) == stdN);
    return 0;
}

i32 threadSleepFor5msTest() {
    core::Expect(core::threadingSleep(5));
    return 0;
}

i32 threadNameingTest() {
    char buff[core::MAX_THREAD_NAME_LENGTH];

    {
        core::memset(buff, 0, core::MAX_THREAD_NAME_LENGTH);
        CT_CHECK(core::threadingGetName(buff).hasErr() == false);
    }

    {
        Expect(core::threadingSetName("First Name"));
        core::memset(buff, 0, core::MAX_THREAD_NAME_LENGTH);
        Expect(core::threadingGetName(buff));
        CT_CHECK(core::memcmp(buff, core::cstrLen(buff), "First Name", core::cstrLen("First Name")) == 0);
    }

    {
        Expect(core::threadingSetName("Name Change"));
        core::memset(buff, 0, core::MAX_THREAD_NAME_LENGTH);
        Expect(core::threadingGetName(buff));
        CT_CHECK(core::memcmp(buff, core::cstrLen(buff), "Name Change", core::cstrLen("Name Change")) == 0);
    }

    return 0;
}

i32 threadStartingARunningThreadReturnsErrorTest() {
    core::Thread t;

    Expect(core::threadInit(t));
    Expect(core::threadStart(t, nullptr, [](void*) { core::threadingSleep(100); }));
    CT_CHECK(core::threadIsRunning(t));
    {
        auto err = core::threadStart(t, nullptr, nullptr);
        CT_CHECK(err.hasErr(), "Starting a thread with a nullptr run function should fail.");
        CT_CHECK(err.err() == core::ERR_INVALID_ARGUMENT, "Invalid error code.");
    }
    {
        auto err = core::threadStart(t, nullptr, [](void*) { core::threadingSleep(100); });
        CT_CHECK(err.hasErr(), "Starting a running thread should fail.");
        CT_CHECK(err.err() == core::ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD, "Invalid error code.");
    }

    Expect(core::threadJoin(t));

    return 0;
}

i32 threadDetachDoesNotBreakTest() {
    core::Thread t;

    // auto res = core::threadDetach(t); NOTE: detach behavior is undefined if the thread is not initialized.

    Expect(core::threadInit(t));
    {
        auto res = core::threadDetach(t);
        CT_CHECK(res.hasErr(), "Detaching a thread that is not running should fail.");
        CT_CHECK(res.err() == core::ERR_THREAD_IS_NOT_JOINABLE_OR_DETACHABLE, "Invalid error code.");
    }

    Expect(core::threadStart(t, nullptr, [](void*) { core::threadingSleep(100); }));
    {
        auto res = core::threadDetach(t);
        CT_CHECK(!res.hasErr(), "Failed to detach thread.");
    }

    return 0;
}

i32 getCurrentThreadTest() {
    core::Thread t;
    Expect(core::threadInit(t));
    Expect(core::threadingGetCurrent(t));
    CT_CHECK(core::threadIsRunning(t));
    return 0;
}

i32 start2ThreadsAndJoinThemTest() {
    i32 iterations = 10;

    while (iterations--) {

        core::Thread t1;
        core::Thread t2;
        bool t1Done = false;
        bool t2Done = false;

        struct Args {
            bool* done;
        };

        // auto res = core::threadStart(t1, nullptr, [](void*) {}); // NOTE: Starting a thread that is not initialized is undefined behavior.

        Expect(core::threadInit(t1));
        Expect(core::threadInit(t2));

        CT_CHECK(core::threadIsRunning(t1) == false);
        CT_CHECK(core::threadIsRunning(t2) == false);

        // IMPORTANT:
        // In this case arguments could be on the stack, because the threads are joined before the function returns, but
        // in general started threads will outlive the current stack frame, dus the lifetime of the arguments must be
        // managed on the heap.
        Args* t1Args = reinterpret_cast<Args*>(core::alloc(1, sizeof(Args)));
        CT_CHECK(t1Args != nullptr);
        *t1Args = {&t1Done};
        defer { core::free(t1Args, 1, sizeof(Args)); };
        Args* t2Args = reinterpret_cast<Args*>(core::alloc(1, sizeof(Args)));
        CT_CHECK(t2Args != nullptr);
        *t2Args = {&t2Done};
        defer { core::free(t2Args, 1, sizeof(Args)); };

        {
            auto res = core::threadStart(t1, reinterpret_cast<void*>(t1Args), [](void* arg) {
                Args* args = reinterpret_cast<Args*>(arg);
                *args->done = true;
            });
            CT_CHECK(!res.hasErr());
        }

        {
            auto res = core::threadStart(t2, reinterpret_cast<void*>(t2Args), [](void* arg) {
                Args* args = reinterpret_cast<Args*>(arg);
                *args->done = true;
            });
            CT_CHECK(!res.hasErr());
        }

        Expect(core::threadJoin(t1));
        CT_CHECK(core::threadIsRunning(t1) == false);
        CT_CHECK(t1Done == true);

        Expect(core::threadJoin(t2));
        CT_CHECK(core::threadIsRunning(t2) == false);
        CT_CHECK(t2Done == true);
    }

    return 0;
}

i32 mutexPreventsRaceConditionsTest() {
    core::Thread t1;
    core::Thread t2;
    i32 counter = 0;
    static core::Mutex mu;

    struct Args {
        i32* counter;
    };

    Expect(core::threadInit(t1));
    Expect(core::threadInit(t2));
    Expect(core::mutexInit(mu));
    defer { Expect(core::mutexDestroy(mu)); };

    Args* t1Args = reinterpret_cast<Args*>(core::alloc(1, sizeof(Args)));
    *t1Args = Args{&counter};
    defer { core::free(t1Args, 1, sizeof(Args)); };
    Args* t2Args = reinterpret_cast<Args*>(core::alloc(1, sizeof(Args)));
    *t2Args = Args{&counter};
    defer { core::free(t2Args, 1, sizeof(Args)); };

    {
        auto res = core::threadStart(t1, reinterpret_cast<void*>(t1Args), [](void* arg) {
            Args* args = reinterpret_cast<Args*>(arg);
            for (i32 i = 0; i < 100000; ++i) {
                Expect(core::mutexLock(mu));

                i32& c = *args->counter;
                ++c;

                Expect(core::mutexUnlock(mu));
            }
        });
        CT_CHECK(!res.hasErr());
    }

    {
        auto res = core::threadStart(t2, reinterpret_cast<void*>(t2Args), [](void* arg) {
            Args* args = reinterpret_cast<Args*>(arg);
            for (i32 i = 0; i < 100000; ++i) {
                Expect(core::mutexLock(mu));

                i32& c = *args->counter;
                ++c;

                Expect(core::mutexUnlock(mu));
            }
        });
        CT_CHECK(!res.hasErr());
    }

    Expect(core::threadJoin(t1));
    Expect(core::threadJoin(t2));

    CT_CHECK(counter == 200000, "Race condition was not prevented by the mutex.");

    return 0;
}

i32 arrayOfMutexesAndThreadsTest() {
    constexpr addr_size N = 4;
    core::ArrList<core::Mutex> mutexes (N);
    core::ArrList<core::Thread> threads (N);

    // Init all objects.
    for (addr_size i = 0; i < N; ++i) {
        mutexes.push(core::Mutex{});
        threads.push(core::Thread{});
        Expect(core::mutexInit(mutexes[i]));
        Expect(core::threadInit(threads[i]));
    }

    // Start all threads.
    for (addr_size i = 0; i < N; ++i) {
        // Check that the threads can be started.
        Expect(core::threadStart(threads[i], nullptr, [](void*) { core::threadingSleep(5); }));

        // Check that the mutexes can be locked and unlocked.
        Expect(core::mutexLock(mutexes[i]));
        Expect(core::mutexUnlock(mutexes[i]));
    }

    // Join all threads.
    for (addr_size i = 0; i < N; ++i) {
        auto& curr = threads[i];
        Expect(core::threadJoin(curr));
    }

    // Check that they are all not running.
    for (addr_size i = 0; i < N; ++i) {
        auto& curr = threads[i];
        CT_CHECK(core::threadIsRunning(curr) == false);
    }

    // Free the mutexes.
    for (addr_size i = 0; i < N; ++i) {
        auto& curr = mutexes[i];
        Expect(core::mutexDestroy(curr));
    }

    return 0;
}

i32 condVarSignalThreadToContinueWorkTest() {
    core::Thread t;
    core::CondVariable cv;

    Expect(core::threadInit(t));
    Expect(core::condVarInit(cv));
    defer { Expect(core::condVarDestroy(cv)); };

    core::threadStart(t, &cv, [](void* arg) {
        core::CondVariable* cvar = reinterpret_cast<core::CondVariable*>(arg);
        core::Mutex mu;

        Expect(core::mutexInit(mu));
        defer { Expect(core::mutexDestroy(mu)); };

        Expect(core::mutexLock(mu));
        defer { Expect(core::mutexUnlock(mu)); };

        // Wait for signal
        Expect(core::condVarWaitTimed(*cvar, mu, 1000)); // 1 second
    });

    Expect(core::threadingSleep(50));

    Expect(core::condVarSignal(cv));

    Expect(core::threadJoin(t));

    return 0;
}

i32 runPltThreadingTestsSuite() {
    using namespace core::testing;

    // NOTE: Not using other memory allocators here because these tests take a bit of time, but more importantly they
    //       override the active context based allocator mechanism and use the default allocator.

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(getNumberOfCoresTest);
    if (runTest(tInfo, getNumberOfCoresTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(threadSleepFor5msTest);
    if (runTest(tInfo, threadSleepFor5msTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(threadNameingTest);
    if (runTest(tInfo, threadNameingTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(threadStartingARunningThreadReturnsErrorTest);
    if (runTest(tInfo, threadStartingARunningThreadReturnsErrorTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(threadDetachDoesNotBreakTest);
    if (runTest(tInfo, threadDetachDoesNotBreakTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(getCurrentThreadTest);
    if (runTest(tInfo, getCurrentThreadTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(start2ThreadsAndJoinThemTest);
    if (runTest(tInfo, start2ThreadsAndJoinThemTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(mutexPreventsRaceConditionsTest);
    if (runTest(tInfo, mutexPreventsRaceConditionsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(arrayOfMutexesAndThreadsTest);
    if (runTest(tInfo, arrayOfMutexesAndThreadsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(condVarSignalThreadToContinueWorkTest);
    if (runTest(tInfo, condVarSignalThreadToContinueWorkTest) != 0) { ret = -1; }

    return ret;
}
