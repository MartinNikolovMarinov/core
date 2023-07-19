i32 basic_std_allocator_case() {
    core::std_allocator stdAlloc;

    void* data = stdAlloc.alloc(4);
    Assert(data != nullptr);
    Assert(core::cptr_len(stdAlloc.allocator_name()) > 0);
    stdAlloc.free(data);

    struct test_struct {
        i32 a;
        f32 b;

        test_struct() : a(0), b(0.0f) {}
        test_struct(i32 a, f32 b) : a(a), b(b) {}
    };

    test_struct* ts = nullptr;
    ts = core::construct(stdAlloc, test_struct{}, 42, 0.1f);
    Assert(ts != nullptr);
    Assert(ts->a == 42);
    Assert(ts->b == 0.1f);
    stdAlloc.free(ts);

    return 0;
}

i32 on_oom_std_allocator() {
    static i32 testOOMCount = 0;
    core::std_allocator stdAlloc([](void*) { testOOMCount++; });
    testOOMCount = 0; // just in case

    [[maybe_unused]] void* data = stdAlloc.alloc(0x7fffffffffffffff);
    Assert(testOOMCount == 1);

    return 0;
}

i32 stats_allocator_tests() {
    core::std_stats_allocator statsStdAllocator;

    struct test_struct {
        i32 a;
        f32 b;

        test_struct() : a(0), b(0.0f) {}
        test_struct(i32 a, f32 b) : a(a), b(b) {}
    };

    test_struct* ts = nullptr;
    ts = core::construct(statsStdAllocator, test_struct{}, 42, 0.1f);
    Assert(ts != nullptr);
    Assert(ts->a == 42);
    Assert(ts->b == 0.1f);
    Assert(statsStdAllocator.used_mem() == sizeof(test_struct));

    statsStdAllocator.free(ts);
    Assert(statsStdAllocator.used_mem() == 0);

    constexpr i32 testCount = 100;
    i32* tcases[testCount]; core::memset(tcases, 0, sizeof(tcases));
    for (i32 i = 0; i < testCount; ++i) {
        tcases[i] = core::construct(statsStdAllocator, i32{}, i);
    }

    Assert(statsStdAllocator.used_mem() == sizeof(i32) * testCount);
    statsStdAllocator.clear();
    Assert(statsStdAllocator.used_mem() == 0);

    // test case with different sizes
    i32* a = nullptr;
    f64* b = nullptr;
    test_struct* c = nullptr;
    a = core::construct(statsStdAllocator, i32{}, 1);
    b = core::construct(statsStdAllocator, f64{}, 2.5);
    c = core::construct(statsStdAllocator, test_struct{}, 3, 3.5f);

    Assert(statsStdAllocator.used_mem() == sizeof(i32) + sizeof(f64) + sizeof(test_struct));
    Assert(*a == 1);
    Assert(*b == 2.5);
    Assert((*c).a == 3);
    Assert((*c).b == 3.5f);

    statsStdAllocator.free(a);
    Assert(statsStdAllocator.used_mem() == sizeof(f64) + sizeof(test_struct));
    statsStdAllocator.free(b);
    Assert(statsStdAllocator.used_mem() == sizeof(test_struct));
    statsStdAllocator.free(c);
    Assert(statsStdAllocator.used_mem() == 0);

    return 0;
}

i32 run_std_allocator_tests_suite() {
    RunTest(basic_std_allocator_case);
    RunTest(on_oom_std_allocator);
    RunTest(stats_allocator_tests);

    // Array with std allocator tests:
    RunTest(initialize_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(resize_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(fill_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(append_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(move_and_copy_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(array_of_arrays_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(clear_array_should_call_dtors_test<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");

    // Flag parser tests:
    RunTest(friendly_input_flag_parser_test<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(adverse_input_flag_parser_test<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(custom_rule_flag_parser_test<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");

    return 0;
}
