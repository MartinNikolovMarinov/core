void basic_bump_allocator_case() {
    core::bump_allocator<254> bumpAllocator;

    void* data = bumpAllocator.alloc(4);
    Assert(data != nullptr);
    Assert(core::cptr_len(bumpAllocator.allocator_name()) > 0);
    bumpAllocator.free(data);

    struct Test {
        i32 a;
        f32 b;

        Test() : a(0), b(0.0f) {}
        Test(i32 a, f32 b) : a(a), b(b) {}
    };

    Test* ts = nullptr;
    ts = core::construct(bumpAllocator, ts, 42, 0.1f);
    Assert(ts != nullptr);
    Assert(ts->a == 42);
    Assert(ts->b == 0.1f);
}

void on_oom_bump_allocator() {
    static i32 g_test_oom_count = 0;
    core::bump_allocator<254> bumpAllocator([](void*) { g_test_oom_count++; });
    g_test_oom_count = 0; // just in case

    [[maybe_unused]] void* data = bumpAllocator.alloc(255);
    Assert(g_test_oom_count > 0);
}

void run_bump_allocator_tests_suite() {
    RunTest(basic_bump_allocator_case);
    RunTest(on_oom_bump_allocator);

    // Array with bump allocator tests
    RunTest(initialize_arr<bump_allocator_static<254>>);
    bump_allocator_static<254>::clear();
    RunTest(resize_arr<bump_allocator_static<254>>);
    bump_allocator_static<254>::clear();
    RunTest(fill_arr<bump_allocator_static<254>>);
    bump_allocator_static<254>::clear();
    RunTest(append_arr<bump_allocator_static<254>>);
    bump_allocator_static<254>::clear();
}
