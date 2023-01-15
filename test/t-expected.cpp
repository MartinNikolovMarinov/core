void expected_basic_case() {
    core::expected<i32, const char*> e1(10);
    Assert(e1.has_value());
    Assert(!e1.has_err());
    Assert(e1.value() == 10);
    Assert(e1.value_or_die() == 10);

    core::expected<i32, const char*> e2(core::unexpected("bad"));
    Assert(!e2.has_value());
    Assert(e2.has_err());
    Assert(core::cptr_cmp(e2.err(), "bad") == 0);
}

void expected_sizeof() {
    struct test_struct {
        u64 a;
        u32 b;
        u16 c;
        i8 d;
        char test[18];
    };

    core::expected<u64, test_struct> e1(1);
    core::expected<u64, test_struct> e2(core::unexpected(test_struct{1,2,3,4,"123"}));
    core::expected<test_struct, u64> e3(test_struct{1,2,3,4,"123"});
    core::expected<test_struct, u64> e4(core::unexpected(1));

    Assert(sizeof(e1) == sizeof(e2));
    Assert(sizeof(e1) == sizeof(e3));
    Assert(sizeof(e1) == sizeof(e4));
}

void expected_with_same_type() {
    core::expected<i32, i32> e1(10);
    Assert(e1.has_value());
    Assert(!e1.has_err());
    Assert(e1.value() == 10);
    Assert(e1.value_or_die() == 10);

    struct test_struct {
        u64 a;
        u32 b;
        u16 c;
        i8 d;
        char test[18];
    };

    core::expected<test_struct, test_struct> e2(test_struct{1,2,3,4,"123"});
    Assert(e2.has_value());
    Assert(!e2.has_err());

    core::expected<test_struct, test_struct> e3(core::unexpected(test_struct{1,2,3,4,"123"}));
    Assert(!e3.has_value());
    Assert(e3.has_err());
}

void expected_used_in_a_function() {
    static constexpr const char* errMsg1 = "unexpected value less than 0";
    static constexpr const char* errMsg2 = "unexpected value equals 0";

    auto f = [](i32 v) -> core::expected<i32, const char*> {
        if (v < 0)       return core::unexpected(errMsg1);
        else if (v == 0) return core::unexpected(errMsg2);
        else             return v + 2;
    };

    Assert(f(5).value_or_die() == 5 + 2);
    Assert(f(0).has_err());
    Assert(core::cptr_cmp(f(0).err(), errMsg2) == 0);
    Assert(f(-1).has_err());
    Assert(core::cptr_cmp(f(-1).err(), errMsg1) == 0);
}

void expected_with_destructors() {
    struct test_struct {
        i32* counter;
        test_struct(i32* counter) : counter(counter) {
            (*this->counter)++;
        }
        test_struct(test_struct&& other) : counter(other.counter) {
            (*this->counter)++;
        }
        ~test_struct() { (*this->counter)--; }
    };

    i32 counter = 0;
    Assert(counter == 0);
    {
        core::expected<test_struct, test_struct> e1(test_struct{&counter});
        Assert(counter == 1);
        {
            core::expected<test_struct, test_struct> e2(core::unexpected(test_struct{&counter}));
            Assert(counter == 2);
        }
        Assert(counter == 1);
    }
    Assert(counter == 0);
}

void run_expected_tests_suite() {
    RunTest(expected_basic_case);
    RunTest(expected_sizeof);
    RunTest(expected_with_same_type);
    RunTest(expected_used_in_a_function);
    RunTest(expected_with_destructors);
}
