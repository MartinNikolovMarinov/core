i32 basic_bump_allocator_case() {
    core::bump_allocator<254> bumpAllocator;

    void* data = bumpAllocator.alloc(4);
    Assert(data != nullptr);
    Assert(core::cptr_len(bumpAllocator.allocator_name()) > 0);
    bumpAllocator.free(data);

    struct test_struct {
        i32 a;
        f32 b;

        test_struct() : a(0), b(0.0f) {}
        test_struct(i32 a, f32 b) : a(a), b(b) {}
    };

    test_struct* ts = nullptr;
    ts = core::construct(bumpAllocator, test_struct{}, 42, 0.1f);
    Assert(ts != nullptr);
    Assert(ts->a == 42);
    Assert(ts->b == 0.1f);
    bumpAllocator.free(ts);

    ts = reinterpret_cast<test_struct*>(bumpAllocator.calloc(1, sizeof(test_struct)));
    Assert(ts != nullptr);
    Assert(ts->a == 0);
    Assert(ts->b == 0.0f);
    bumpAllocator.free(ts);

    return 0;
}

i32 on_oom_bump_allocator() {
    static i32 testOOMCount = 0;
    core::bump_allocator<254> bumpAllocator([](void*) { testOOMCount++; });
    testOOMCount = 0; // just in case

    [[maybe_unused]] void* data = bumpAllocator.alloc(255);
    Assert(testOOMCount > 0);

    return 0;
}

i32 run_bump_allocator_tests_suite() {
    RunTest(basic_bump_allocator_case);
    RunTest(on_oom_bump_allocator);

    // Array with bump allocator tests:
    RunTest(initialize_arr<bump_allocator_static<512>>);
    bump_allocator_static<512>::clear();
    RunTest(resize_arr<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(fill_arr<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(append_arr<bump_allocator_static<512>>);
    bump_allocator_static<512>::clear();
    RunTest(move_and_copy_arr<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(array_of_arrays_arr<bump_allocator_static<512>>);
    bump_allocator_static<512>::clear();
    RunTest(clear_array_should_call_dtors_test<bump_allocator_static<512>>);
    bump_allocator_static<512>::clear();

    // Flag parser tests:
    RunTest(friendly_input_flag_parser_test<bump_allocator_static<4096>>);
    bump_allocator_static<4096>::clear();
    RunTest(adverse_input_flag_parser_test<bump_allocator_static<4096>>);
    bump_allocator_static<4096>::clear();
    RunTest(custom_rule_flag_parser_test<bump_allocator_static<4096>>);
    bump_allocator_static<4096>::clear();

    // String tests:
    RunTest(initalize_str_builder<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(move_and_copy_str_builder<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(resize_str_builder<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(append_str_builder<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(take_and_steal_str_builder<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();
    RunTest(special_cases_related_to_null_termination_str_builder<bump_allocator_static<256>>);
    bump_allocator_static<256>::clear();

    // Hash map tests:
    RunTest(initialize_hash_map<bump_allocator_static<512>>);
    bump_allocator_static<512>::clear();
    RunTest(put_move_copy_hash_map<bump_allocator_static<4096>>);
    bump_allocator_static<4096>::clear();
    RunTest(remove_from_hash_map<bump_allocator_static<1024>>);
    bump_allocator_static<1024>::clear();
    RunTest(complex_types_in_hash_map<bump_allocator_static<1024>>);
    bump_allocator_static<1024>::clear();

    return 0;
}
