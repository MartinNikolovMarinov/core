#include "t-index.h"

PRAGMA_WARNING_PUSH

DISABLE_MSVC_WARNING(4127) // Conditional expression is constant. I don't care here.

constexpr i32 expectedBasicCaseTest() {
    core::expected<i32, const char*> e1(10);
    CT_CHECK(e1.hasValue());
    CT_CHECK(!e1.hasErr());
    CT_CHECK(e1.value() == 10);
    auto v = core::Unpack(std::move(e1));
    CT_CHECK(v == 10);

    core::expected<i32, const char*> e2(core::unexpected("bad"));
    CT_CHECK(!e2.hasValue());
    CT_CHECK(e2.hasErr());
    CT_CHECK(core::cptrEq(e2.err(), "bad", core::cptrLen(e2.err())));

    core::expected<i32> e3;
    CT_CHECK(!e3.hasErr());
    core::Expect(std::move(e3)); // should not crash!

    core::expected<i32> e4(core::unexpected(7));
    CT_CHECK(e4.hasErr());
    CT_CHECK(e4.err() == 7);
    // core::Expect(std::move(e4)); // will crash!

    auto e5 = std::move(e4);
    CT_CHECK(e5.hasErr());
    CT_CHECK(e5.err() == 7);

    return 0;
}

// This can't be constexpr because of using structs with non-trivial destructors.
i32 expectedWithDestructor() {
    using namespace core::testing;

    {
        defer { CT::resetAll(); };

        CT ct;
        ct.a = 90;
        CT::resetAll(); // discount the constructors called by the CT creation

        {
            core::expected<CT, i32> e1 (std::move(ct));
            CT_CHECK(e1.hasValue());
            CT_CHECK(!e1.hasErr());
            CT_CHECK(e1.value().a == 90);
            CT_CHECK(CT::moveCtorCalled() == 1);
            CT_CHECK(CT::copyCtorCalled() == 0);
            CT_CHECK(CT::defaultCtorCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            CT::resetCtors();

            {
                // After this move, e1 will still call its destructor, but the value will have been moved.
                auto v = core::Unpack(std::move(e1));
                CT_CHECK(v.a == 90);
                CT_CHECK(CT::moveCtorCalled() == 1);
                CT_CHECK(CT::copyCtorCalled() == 0);
                CT_CHECK(CT::defaultCtorCalled() == 0);
                CT_CHECK(CT::dtorsCalled() == 0);
                CT_CHECK(CT::assignmentsTotalCalled() == 0);
            }
            CT_CHECK(CT::dtorsCalled() == 1); // this is where CT is actually destroyed
            CT_CHECK(ct.a == CT::defaultValue); // destructor has been called

            CT::resetCtors();

            {
                core::expected<CT, i32> e2 (core::unexpected(1));
                CT_CHECK(CT::noCtorsCalled());
                CT_CHECK(CT::noAssignmentsCalled());
                CT_CHECK(CT::dtorsCalled() == 1);

                auto v2 = std::move(e2);
                CT_CHECK(v2.hasErr());
                CT_CHECK(!v2.hasValue());
                CT_CHECK(v2.err() == 1);
                CT_CHECK(CT::noCtorsCalled());
                CT_CHECK(CT::noAssignmentsCalled());
                CT_CHECK(CT::dtorsCalled() == 1);
            }
            CT_CHECK(CT::dtorsCalled() == 1); // The above should not call any destructors!
        }
        CT_CHECK(CT::dtorsCalled() == 2);
        CT::resetAll();
    }

    return 0;
}

constexpr i32 expectedSizeofTest() {
    struct TestStruct {
        u64 a;
        u32 b;
        u16 c;
        i8 d;
        char test[18];
    };

    core::expected<u64, TestStruct> e1(1);
    core::expected<u64, TestStruct> e2(core::unexpected(TestStruct{1,2,3,4,"123"}));
    core::expected<TestStruct, u64> e3(TestStruct{1,2,3,4,"123"});
    core::expected<TestStruct, u64> e4(core::unexpected(u64(1)));

    CT_CHECK(sizeof(e1) == sizeof(e2));
    CT_CHECK(sizeof(e1) == sizeof(e3));
    CT_CHECK(sizeof(e1) == sizeof(e4));

    core::expected<TestStruct> e5;
    core::expected<TestStruct> e6(core::unexpected(TestStruct{1,2,3,4,"123"}));

    CT_CHECK(sizeof(e5) == sizeof(e6));

    return 0;
}

constexpr i32 expectedWithSameTypeTest() {
    core::expected<i32, i32> e1(10);
    CT_CHECK(e1.hasValue());
    CT_CHECK(!e1.hasErr());
    CT_CHECK(e1.value() == 10);
    CT_CHECK(core::Unpack(std::move(e1)) == 10);

    struct TestStruct {
        u64 a;
        u32 b;
        u16 c;
        i8 d;
        char test[18];
    };

    core::expected<TestStruct, TestStruct> e2(TestStruct{1,2,3,4,"123"});
    CT_CHECK(e2.hasValue());
    CT_CHECK(!e2.hasErr());

    core::expected<TestStruct, TestStruct> e3(core::unexpected(TestStruct{1,2,3,4,"123"}));
    CT_CHECK(!e3.hasValue());
    CT_CHECK(e3.hasErr());

    return 0;
}

constexpr i32 expectedUsedInAFunctionTest() {
    constexpr const char* errMsg1 = "unexpected value less than 0";
    constexpr const char* errMsg2 = "unexpected value equals 0";

    auto f = [&](i32 v) -> core::expected<i32, const char*> {
        if (v < 0)       return core::unexpected(errMsg1);
        else if (v == 0) return core::unexpected(errMsg2);
        else             return v + 2;
    };

    CT_CHECK(core::Unpack(f(5)) == 5 + 2);
    CT_CHECK(f(0).hasErr());
    CT_CHECK(core::cptrEq(f(0).err(), errMsg2, core::cptrLen(errMsg2)));
    CT_CHECK(f(-1).hasErr());
    CT_CHECK(core::cptrEq(f(-1).err(), errMsg1, core::cptrLen(errMsg1)));

    return 0;
}

i32 expectedWithDestructorsTest() {
    struct TestStruct {
        i32* counter;
        TestStruct(i32* _counter) : counter(_counter) {
            (*this->counter)++;
        }
        TestStruct(TestStruct&& other) : counter(other.counter) {
            (*this->counter)++;
        }
        ~TestStruct() { (*this->counter)--; }
    };

    i32 counter = 0;
    CT_CHECK(counter == 0);
    {
        core::expected<TestStruct, TestStruct> e1(TestStruct{&counter});
        CT_CHECK(counter == 1);
        {
            core::expected<TestStruct, TestStruct> e2(core::unexpected(TestStruct{&counter}));
            CT_CHECK(counter == 2);
        }
        CT_CHECK(counter == 1);
    }
    CT_CHECK(counter == 0);

    return 0;
}

PRAGMA_WARNING_POP

i32 runExpectedTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(expectedBasicCaseTest);
    if (runTest(tInfo, expectedBasicCaseTest) != 0) { ret = -1;}
    tInfo.name = FN_NAME_TO_CPTR(expectedWithDestructor);
    if (runTest(tInfo, expectedWithDestructor) != 0) { ret = -1;}
    tInfo.name = FN_NAME_TO_CPTR(expectedSizeofTest);
    if (runTest(tInfo, expectedSizeofTest) != 0) { ret = -1;}
    tInfo.name = FN_NAME_TO_CPTR(expectedWithSameTypeTest);
    if (runTest(tInfo, expectedWithSameTypeTest) != 0) { ret = -1;}
    tInfo.name = FN_NAME_TO_CPTR(expectedUsedInAFunctionTest);
    if (runTest(tInfo, expectedUsedInAFunctionTest) != 0) { ret = -1;}
    tInfo.name = FN_NAME_TO_CPTR(expectedWithDestructorsTest);
    if (runTest(tInfo, expectedWithDestructorsTest) != 0) { ret = -1;}

    return ret;
}

constexpr i32 runCompiletimeExpectedTestsSuite() {
    RunTestCompileTime(expectedBasicCaseTest);
    RunTestCompileTime(expectedSizeofTest);
    RunTestCompileTime(expectedWithSameTypeTest);
    RunTestCompileTime(expectedUsedInAFunctionTest);

    return 0;
}
