template<template<typename T, typename TErr> class TEx>
constexpr i32 expected_basic_case() {
    TEx<i32, const char*> e1(10);
    Assert(e1.has_value());
    Assert(!e1.has_err());
    Assert(e1.value() == 10);
    Assert(ValueOrDie(e1) == 10);

    TEx<i32, const char*> e2(core::unexpected("bad"));
    Assert(!e2.has_value());
    Assert(e2.has_err());
    Assert(core::cptr_cmp(e2.err(), "bad") == 0);

    return 0;
}

template<template<typename T, typename TErr> class TEx>
constexpr i32 expected_sizeof() {
    struct test_struct {
        u64 a;
        u32 b;
        u16 c;
        i8 d;
        char test[18];
    };

    TEx<u64, test_struct> e1(1);
    TEx<u64, test_struct> e2(core::unexpected(test_struct{1,2,3,4,"123"}));
    TEx<test_struct, u64> e3(test_struct{1,2,3,4,"123"});
    TEx<test_struct, u64> e4(core::unexpected(1));

    Assert(sizeof(e1) == sizeof(e2));
    Assert(sizeof(e1) == sizeof(e3));
    Assert(sizeof(e1) == sizeof(e4));

    return 0;
}

template<template<typename T, typename TErr> class TEx>
constexpr i32 expected_with_same_type() {
    TEx<i32, i32> e1(10);
    Assert(e1.has_value());
    Assert(!e1.has_err());
    Assert(e1.value() == 10);
    Assert(ValueOrDie(e1) == 10);

    struct test_struct {
        u64 a;
        u32 b;
        u16 c;
        i8 d;
        char test[18];
    };

    TEx<test_struct, test_struct> e2(test_struct{1,2,3,4,"123"});
    Assert(e2.has_value());
    Assert(!e2.has_err());

    TEx<test_struct, test_struct> e3(core::unexpected(test_struct{1,2,3,4,"123"}));
    Assert(!e3.has_value());
    Assert(e3.has_err());

    return 0;
}

template<template<typename T, typename TErr> class TEx>
constexpr i32 expected_used_in_a_function() {
    constexpr const char* errMsg1 = "unexpected value less than 0";
    constexpr const char* errMsg2 = "unexpected value equals 0";

    auto f = [&](i32 v) -> TEx<i32, const char*> {
        if (v < 0)       return core::unexpected(errMsg1);
        else if (v == 0) return core::unexpected(errMsg2);
        else             return v + 2;
    };

    Assert(ValueOrDie(f(5)) == 5 + 2);
    Assert(f(0).has_err());
    Assert(core::cptr_cmp(f(0).err(), errMsg2) == 0);
    Assert(f(-1).has_err());
    Assert(core::cptr_cmp(f(-1).err(), errMsg1) == 0);

    return 0;
}

template<template<typename T, typename TErr> class TEx>
constexpr i32 expected_with_destructors() {
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
        TEx<test_struct, test_struct> e1(test_struct{&counter});
        Assert(counter == 1);
        {
            TEx<test_struct, test_struct> e2(core::unexpected(test_struct{&counter}));
            Assert(counter == 2);
        }
        Assert(counter == 1);
    }
    Assert(counter == 0);

    return 0;
}

i32 run_expected_tests_suite() {
    RunTest(expected_basic_case<core::expected>);
    RunTest(expected_sizeof<core::expected>);
    RunTest(expected_with_same_type<core::expected>);
    RunTest(expected_used_in_a_function<core::expected>);
    RunTest(expected_with_destructors<core::expected>);

    return 0;
}

constexpr i32 run_compiletime_expected_tests_suite() {
    RunTestCompileTime(expected_basic_case<core::static_expected>);
    RunTestCompileTime(expected_sizeof<core::static_expected>);
    RunTestCompileTime(expected_with_same_type<core::static_expected>);
    RunTestCompileTime(expected_used_in_a_function<core::static_expected>);

    return 0;
}
