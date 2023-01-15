void basic_std_allocator_case() {
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
    ts = core::construct(stdAlloc, ts, 42, 0.1f);
    Assert(ts != nullptr);
    Assert(ts->a == 42);
    Assert(ts->b == 0.1f);
}

void on_oom_std_allocator() {
    static i32 testOOMCount = 0;
    core::std_allocator stdAlloc([](void*) { testOOMCount++; });
    testOOMCount = 0; // just in case

    [[maybe_unused]] void* data = stdAlloc.alloc(0x7fffffffffffffff);
    Assert(testOOMCount == 1);
}

void run_std_allocator_tests_suite() {
    RunTest(basic_std_allocator_case);
    RunTest(on_oom_std_allocator);

     // Array with std allocator tests:
    RunTest(initialize_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(resize_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(fill_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
    RunTest(append_arr<std_allocator_static>);
    Assert(std_allocator_static::used_mem() == 0, "memory leak detected");
}
