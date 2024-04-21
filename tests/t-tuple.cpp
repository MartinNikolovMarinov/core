#include "t-index.h"

struct A {
    i32 a;
    f64 b;

    constexpr A(i32 _a, f64 _b) : a(_a), b(_b) {}

    constexpr bool operator==(const A& other) const { return a == other.a && b == other.b; }
    constexpr bool operator!=(const A& other) const { return !(*this == other); }
};

constexpr i32 runTupleArgumentIncrementTest() {
    auto t1 = core::createTuple(1, A{2, 3.0}, u64(6));
    auto t2 = t1;

    CT_CHECK(t1.get<0>() == t2.get<0>());
    CT_CHECK(t1.get<1>() == t2.get<1>());
    CT_CHECK(t1.get<2>() == t2.get<2>());

    CT_CHECK(t1.get<2>() == 6);
    auto& ref = t1.get<2>();
    ref++;
    CT_CHECK(t1.get<2>() == 7);
    auto& [a, b, c] = t1;
    c++;
    CT_CHECK(t1.get<2>() == 8);

    CT_CHECK(t1.get<0>() == t2.get<0>());
    CT_CHECK(t1.get<1>() == t2.get<1>());
    CT_CHECK(t1.get<2>() != t2.get<2>());

    return 0;
}

PRAGMA_WARNING_PUSH

DISABLE_MSVC_WARNING(4127) // Disable conditional expression is constant. This is intentional here.

constexpr i32 runCreateTuplesOfDifferentSizesTest() {
    {
        auto t = core::createTuple(1, 2);
        CT_CHECK(t.len == 2);
        CT_CHECK(t.get<0>() == 1);
        CT_CHECK(t.get<1>() == 2);
        auto [first, second] = t;
        CT_CHECK(first == 1);
        CT_CHECK(second == 2);
    }
    {
        auto t = core::createTuple(1, 2, 3);
        CT_CHECK(t.len == 3);
        CT_CHECK(t.get<0>() == 1);
        CT_CHECK(t.get<1>() == 2);
        CT_CHECK(t.get<2>() == 3);
        auto [first, second, third] = t;
        CT_CHECK(first == 1);
        CT_CHECK(second == 2);
        CT_CHECK(third == 3);
    }
    {
        auto t = core::createTuple(1, 2, 3, 4);
        CT_CHECK(t.len == 4);
        CT_CHECK(t.get<0>() == 1);
        CT_CHECK(t.get<1>() == 2);
        CT_CHECK(t.get<2>() == 3);
        CT_CHECK(t.get<3>() == 4);
        auto [first, second, third, fourth] = t;
        CT_CHECK(first == 1);
        CT_CHECK(second == 2);
        CT_CHECK(third == 3);
        CT_CHECK(fourth == 4);
    }

    return 0;
}

PRAGMA_WARNING_POP

i32 runTupleTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(runTupleArgumentIncrementTest);
    if (runTest(tInfo, runTupleArgumentIncrementTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(runCreateTuplesOfDifferentSizesTest);
    if (runTest(tInfo, runCreateTuplesOfDifferentSizesTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeTupleTestsSuite() {
    RunTestCompileTime(runTupleArgumentIncrementTest);
    RunTestCompileTime(runCreateTuplesOfDifferentSizesTest);
    return 0;
}
