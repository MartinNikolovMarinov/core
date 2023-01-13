void basic_case() {
    void* data = core::std_allocator::alloc(4);
    Assert(data != nullptr);
    core::std_allocator::free(data);

    struct Test {
        i32 a;
        f32 b;

        Test() : a(0), b(0.0f) {}
        Test(i32 a, f32 b) : a(a), b(b) {}
    };

    Test* ts = nullptr;
    ts = construct(core::std_allocator{}, ts, 42, 0.1f);
    Assert(ts != nullptr);
    Assert(ts->a == 42);
    Assert(ts->b == 0.1f);
}

void run_alloc_std_tests_suite() {
    RunTest(basic_case);
}
