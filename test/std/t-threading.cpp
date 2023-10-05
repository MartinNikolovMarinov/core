// TODO: [Robustness] I will need to write a lot more tests to make sure the threading system behaves correctly.
// TODO: [Robustness] Some of the tests for threads need a separate test binary to test calling thread_exit or detaching, etc.
// TODO: [Robustness] Test different types of mutexes. ErrorCheck should catch some deadlock errors!
// TODO: [Robustness] Conditional variable timed waits should be tested as well.

// FIXME: Test thread local storage!

i32 get_number_of_cores_test() {
    auto res = core::threading_get_num_cores();
    Assert(!res.has_err())
    Assert(res.value() > 0);
    return 0;
}

i32 thread_sleep_test() {
    Expect(core::threading_sleep(100));
    return 0;
}

i32 get_current_thread_test() {
    core::thread t0;
    core::threading_get_current(t0);
    Assert(core::thread_is_running(t0));
    return 0;
}

i32 thread_eq_test() {
    core::thread t0;
    core::threading_get_current(t0);

    bool areEq = ValueOrDie(core::thread_eq(t0, t0));
    Assert(areEq);

    return 0;
}

i32 threading_get_and_set_name_test() {
    char buf[core::MAX_THREAD_NAME_LENGTH] = {};
    Expect(core::threading_get_name(buf)); // Should not fail
    core::memset(buf, 0, core::MAX_THREAD_NAME_LENGTH);

    Expect(core::threading_set_name("FIRST_NAME"));

    Expect(core::threading_get_name(buf));
    Assert(core::cptr_eq(buf, "FIRST_NAME", core::cptr_len("FIRST_NAME")));
    core::memset(buf, 0, core::MAX_THREAD_NAME_LENGTH);

    Expect(core::threading_set_name("SECOND_NAME"));

    Expect(core::threading_get_name(buf));
    Assert(core::cptr_eq(buf, "SECOND_NAME", core::cptr_len("SECOND_NAME")));
    core::memset(buf, 0, core::MAX_THREAD_NAME_LENGTH);

    return 0;
}

i32 thread_start_and_join_two_basic_threads_test() {
    static core::thread_id t1id = 0;
    static core::thread_id t2id = 0;

    core::thread t1;
    {
        auto err = core::thread_start(t1, nullptr, [](void*) -> void* {
            core::thread currt;
            core::threading_get_current(currt);
            core::thread_id id = core::thread_get_id(currt);
            t1id = id;
            Expect(core::threading_sleep(100));
            return nullptr;
        });
        Expect(err);

        Assert(core::thread_is_running(t1));
    }

    // Try to start the same thread again. Should fail.
    {
        auto err = core::thread_start(t1, nullptr, [](void*) -> void* {
            return nullptr;
        });
        Assert(err.has_err());
        core::plt_err_code errCode = err.err();
        Assert(errCode == core::ERR_THREADING_THREAD_IS_ALREADY_RUNNING);
    }

    core::thread t2;
    {
        auto err = core::thread_start(t2, nullptr, [](void*) -> void* {
            core::thread currt;
            core::threading_get_current(currt);
            core::thread_id id = core::thread_get_id(currt);
            t2id = id;
            Expect(core::threading_sleep(100));
            return nullptr;
        });
        Expect(err);

        Assert(core::thread_is_running(t2));
    }

    {
        auto err = core::thread_join(t1);
        Expect(err);
    }
    {
        auto err = core::thread_join(t2);
        Expect(err);
    }

    Assert(t1id != 0);
    Assert(t2id != 0);
    Assert(t1id != t2id);

    return 0;
}

i32 thread_mutex_basic_sync_test() {
    static constexpr i32 ITERATION_COUNT = 1000000;
    static i32 counter = 0;
    static core::mutex mux;

    core::thread_routine increment = [](void*) -> void* {
        for (i32 i = 0; i < ITERATION_COUNT; i++) {
            Expect(core::mutex_lock(mux));
            counter++;
            Expect(core::mutex_unlock(mux));
        }
        return nullptr;
    };

    Expect(core::mutex_init(mux));

    core::thread t1;
    Expect(core::thread_start(t1, nullptr, increment));

    core::thread t2;
    Expect(core::thread_start(t2, nullptr, increment));

    Expect(core::thread_join(t1));
    Expect(core::thread_join(t2));

    Assert(counter == ITERATION_COUNT * 2);

    Expect(core::mutex_destroy(mux));

    return 0;
}

i32 thread_barrier_basic_progression_sync_test() {
    static constexpr i32 THREAD_COUNT = 5;
    static i32 threadProgress[THREAD_COUNT] = {};
    static i32 threadSleepIntervals[THREAD_COUNT] = { 70, 100, 28, 34, 140 };
    static core::barrier bar;

    core::thread threads[THREAD_COUNT];

    Expect(core::barrier_init(bar, THREAD_COUNT));

    for (i32 i = 0; i < THREAD_COUNT; i++) {
        auto err = core::thread_start(threads[i], reinterpret_cast<void*>(i), [](void* arg) -> void* {
            i64 i = i64(arg);

            threadProgress[i] = 1; // Mark as "Doing some work"
            Expect(core::threading_sleep(threadSleepIntervals[i])); // Do some work

            threadProgress[i] = 2; // Mark as "Done"
            Expect(core::barrier_wait(bar)); // Wait for others

            // All threads should have passed the barrier and done the work at this point.
            // All threads check this fact here:
            for (i32 j = 0; j < THREAD_COUNT; j++) {
                if (i == j) continue; // This is me, skip.
                Assert(threadProgress[j] == 2);
            }

            return nullptr;
        });
        Expect(err);
    }

    for (i32 i = 0; i < THREAD_COUNT; i++) {
        Expect(core::thread_join(threads[i]));
    }

    Expect(core::barrier_destroy(bar));

    return 0;
}

i32 thread_cond_var_basic_signaling_test() {
    static constexpr i32 RESOURCES_COUNT = 15;
    static constexpr i32 MAX_CONSUMED = 10;
    static i32 sharedResource[RESOURCES_COUNT] = {};
    static i32 consumed = 0;
    static core::cond_var cv;
    static core::mutex mux;

    core::thread_routine producer = [](void*) -> void* {
        // Produce resources and notify whoever is waiting for them.
        // If the consumer is wating this will signal it to wake up and consume.
        // If no consumer is wating this should not block or crash!
        for (i32 i = 0; i < RESOURCES_COUNT; i++) {
            Expect(core::mutex_lock(mux));
            sharedResource[i] = (i + 1);
            Expect(core::cond_var_signal(cv));
            Expect(core::mutex_unlock(mux));
        }
        return nullptr;
    };

    core::thread_routine consumer = [](void*) -> void* {
        // Consumes the produced resources up to a limit.
        for (i32 i = 0; i < MAX_CONSUMED; i++) {
            Expect(core::mutex_lock(mux));
            while (sharedResource[i] == 0) {
                Expect(core::cond_var_wait(cv, mux));
            }
            consumed++;
            Expect(core::mutex_unlock(mux));
        }
        return nullptr;
    };

    Expect(core::cond_var_init(cv));
    Expect(core::mutex_init(mux));

    core::thread t1, t2;

    Expect(core::thread_start(t1, nullptr, producer));
    Expect(core::thread_start(t2, &consumer, consumer));

    Expect(core::thread_join(t1));
    Expect(core::thread_join(t2));

    Assert(consumed == MAX_CONSUMED);

    Expect(core::cond_var_destroy(cv));

    return 0;
}

i32 run_threading_tests_suite() {
    RunTest(get_number_of_cores_test);
    RunTest(thread_sleep_test);
    RunTest(get_current_thread_test);
    RunTest(thread_eq_test);
    RunTest(threading_get_and_set_name_test);

    RunTest(thread_start_and_join_two_basic_threads_test);
    RunTest(thread_mutex_basic_sync_test);
    RunTest(thread_barrier_basic_progression_sync_test);
    RunTest(thread_cond_var_basic_signaling_test);

    return 0;
}
