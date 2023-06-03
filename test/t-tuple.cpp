struct A {
    i32 a;
    f64 b;

    A(i32 a, f64 b) : a(a), b(b) {}

    bool operator==(const A& other) const { return a == other.a && b == other.b; }
    bool operator!=(const A& other) const { return !(*this == other); }
};

void run_tuple_argument_increment() {
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
}

void run_create_tuples_of_different_sizes() {
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
}

void run_tuple_tests_suite() {
    RunTest(run_tuple_argument_increment);
    RunTest(run_create_tuples_of_different_sizes);
}
