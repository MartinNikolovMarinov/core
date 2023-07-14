struct A {
    i32 a;
    f64 b;

    constexpr A(i32 a, f64 b) : a(a), b(b) {}

    constexpr bool operator==(const A& other) const { return a == other.a && b == other.b; }
    constexpr bool operator!=(const A& other) const { return !(*this == other); }
};

constexpr i32 run_tuple_argument_increment() {
    auto t1 = core::create_tuple(1, A{2, 3.0f}, (u64)(6));
    auto t2 = t1;

    Assert(t1.get<0>() == t2.get<0>());
    Assert(t1.get<1>() == t2.get<1>());
    Assert(t1.get<2>() == t2.get<2>());

    Assert(t1.get<2>() == 6);
    auto& ref = t1.get<2>();
    ref++;
    Assert(t1.get<2>() == 7);
    auto& [a, b, c] = t1;
    c++;
    Assert(t1.get<2>() == 8);

    Assert(t1.get<0>() == t2.get<0>());
    Assert(t1.get<1>() == t2.get<1>());
    Assert(t1.get<2>() != t2.get<2>());

    return 0;
}

constexpr i32 run_create_tuples_of_different_sizes() {
    {
        auto t = core::create_tuple(1, 2);
        Assert(t.len == 2);
        Assert(t.get<0>() == 1);
        Assert(t.get<1>() == 2);
        auto [first, second] = t;
        Assert(first == 1);
        Assert(second == 2);
    }
    {
        auto t = core::create_tuple(1, 2, 3);
        Assert(t.len == 3);
        Assert(t.get<0>() == 1);
        Assert(t.get<1>() == 2);
        Assert(t.get<2>() == 3);
        auto [first, second, third] = t;
        Assert(first == 1);
        Assert(second == 2);
        Assert(third == 3);
    }
    {
        auto t = core::create_tuple(1, 2, 3, 4);
        Assert(t.len == 4);
        Assert(t.get<0>() == 1);
        Assert(t.get<1>() == 2);
        Assert(t.get<2>() == 3);
        Assert(t.get<3>() == 4);
        auto [first, second, third, fourth] = t;
        Assert(first == 1);
        Assert(second == 2);
        Assert(third == 3);
        Assert(fourth == 4);
    }

    return 0;
}

i32 run_tuple_tests_suite() {
    RunTest(run_tuple_argument_increment);
    RunTest(run_create_tuples_of_different_sizes);
    return 0;
}

constexpr i32 run_constexpr_tuple_tests_suite() {
    RunTestCompileTime(run_tuple_argument_increment);
    RunTestCompileTime(run_create_tuples_of_different_sizes);
    return 0;
}
