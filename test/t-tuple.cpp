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

    Assert(t1.get<0>() == t2.get<0>());
    Assert(t1.get<1>() == t2.get<1>());
    Assert(t1.get<2>() != t2.get<2>());
}

void run_create_tuples_of_different_sizes() {
    auto t1 = core::create_tuple(1);
    auto t2 = core::create_tuple(1, 2);
    auto t3 = core::create_tuple(1, 2, 3);
    auto t4 = core::create_tuple(1, 2, 3, 4);

    Assert(t1.len == 1);
    Assert(t1.get<0>() == 1);
    Assert(t2.len == 2);
    Assert(t2.get<0>() == 1);
    Assert(t2.get<1>() == 2);
    Assert(t3.len == 3);
    Assert(t3.get<0>() == 1);
    Assert(t3.get<1>() == 2);
    Assert(t3.get<2>() == 3);
    Assert(t4.len == 4);
    Assert(t4.get<0>() == 1);
    Assert(t4.get<1>() == 2);
    Assert(t4.get<2>() == 3);
    Assert(t4.get<3>() == 4);
}

void run_tuple_tests_suite() {
    RunTest(run_tuple_argument_increment);
    RunTest(run_create_tuples_of_different_sizes);
}
