#include "t-index.h"
#include "testing/testing_framework.h"

PRAGMA_WARNING_PUSH

DISABLE_MSVC_WARNING(4127) // Conditional expression is constant. I don't care here.

i32 findAlgorithmTest() {
    {
        constexpr addr_size BUFF_LEN = 5;
        i32 arr[BUFF_LEN] = {1, 2, 3, 4, 5};
        addr_off found = core::find(arr, BUFF_LEN, [](i32 elem, addr_size) -> bool { return elem == 3; });
        CT_CHECK(found == 2);
        found = core::find(arr, BUFF_LEN, [] (i32 elem, addr_size) -> bool { return elem == 6; });
        CT_CHECK(found == -1);
    }

    {
        struct TestStruct {
            i32 a;
            i32 b;
        };

        core::ArrList<TestStruct> arr;
        arr.push(TestStruct{1, 2});
        arr.push(TestStruct{3, 4});
        arr.push(TestStruct{5, 6});
        addr_off found = core::find(arr, [](const TestStruct& elem, addr_size) -> bool { return elem.a == 3; });
        CT_CHECK(found == 1);
        found = core::find(arr, [] (const TestStruct& elem, addr_size) -> bool { return elem.a == 6; });
        CT_CHECK(found == -1);
    }

    {
        struct TestCase {
            const i32* src;
            addr_size srcLen;
            i32 val;
            addr_off idx;
        };

        const i32 a1[] = {1};
        constexpr addr_size a1Len = CORE_C_ARRLEN(a1);
        const i32 a2[] = {core::limitMin<i32>(), 0, core::limitMax<i32>()};
        constexpr addr_size a2Len = CORE_C_ARRLEN(a2);
        const i32 a3[] = {core::limitMin<i32>(), 0, core::limitMax<i32>(), core::limitMax<i32>() / 2};
        constexpr addr_size a3Len = CORE_C_ARRLEN(a3);

        TestCase cases[] = {
            { {}, 0, 0, -1 },
            { nullptr, 0, 0, -1 },

            { a1, a1Len, 2, -1 },
            { a1, a1Len, core::limitMax<i32>(), -1 },
            { a1, a1Len, 1, 0 },

            { a2, a2Len, core::limitMin<i32>() + 1, -1 },
            { a2, a2Len, core::limitMin<i32>(), 0 },
            { a2, a2Len, 0, 1 },
            { a2, a2Len, core::limitMax<i32>(), 2 },
            { a2, a2Len, core::limitMax<i32>() - 1, -1 },

            { a3, a3Len, core::limitMin<i32>() + 1, -1 },
            { a3, a3Len, core::limitMin<i32>(), 0 },
            { a3, a3Len, 0, 1 },
            { a3, a3Len, core::limitMax<i32>(), 2 },
            { a3, a3Len, core::limitMax<i32>() / 2, 3 },
            { a3, a3Len, core::limitMax<i32>() / 2 + 1, -1 },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            addr_off idx = core::find(c.src, c.srcLen, [&](i32 a, addr_size) -> bool { return a == c.val; });
            CT_CHECK(idx == c.idx, cErr);
            return 0;
        });

        CT_CHECK(ret == 0);
    }

    {
        struct TestCase {
            const char* src;
            char val;
            addr_off idx;
        };

        TestCase cases[] = {
            { "1234567890", '1', 0 },
            { "1234567890", '2', 1 },
            { "1234567890", '3', 2 },
            { "1234567890", '4', 3 },
            { "1234567890", '5', 4 },
            { "1234567890", '6', 5 },
            { "1234567890", '7', 6 },
            { "1234567890", '8', 7 },
            { "1234567890", '9', 8 },
            { "1234567890", '0', 9 },
            { "1234567890", 'z', -1 },
        };

        i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
            addr_off idx = core::find(c.src, core::cstrLen(c.src), [&](char a, addr_size) -> bool { return a == c.val; });
            CT_CHECK(idx == c.idx, cErr);
            return 0;
        });

        CT_CHECK(ret == 0);
    }

    return 0;
}

i32 forAllAlgorithmTest() {
    {
        i32 arr[] = {1, 2, 3};
        bool ok = core::forAll(arr, CORE_C_ARRLEN(arr), [](i32 v, addr_size) { return v > 0; });
        CT_CHECK(ok);
        ok = core::forAll(arr, CORE_C_ARRLEN(arr), [](i32 v, addr_size) { return v < 3; });
        CT_CHECK(!ok);
    }

    {
        core::ArrList<i32> arr;
        arr.push(1);
        arr.push(2);
        arr.push(3);
        bool ok = core::forAll(arr, [](i32 v, addr_size) { return v > 0; });
        CT_CHECK(ok);
        ok = core::forAll(arr, [](i32 v, addr_size) { return v > 2; });
        CT_CHECK(!ok);
    }

    return 0;
}

i32 basicpushUniqueTest() {
    using namespace core::testing;

    {
        core::ArrList<i32> arr;

        auto eqFn = [](const i32& curr, addr_size, const i32& el) -> bool { return curr == el; };

        core::pushUnique(arr, 1, eqFn);
        core::pushUnique(arr, 2, eqFn);
        i32 nonExistingLValue = 3;
        core::pushUnique(arr, nonExistingLValue, eqFn);
        i32 existingLValue = 1;
        core::pushUnique(arr, existingLValue, eqFn);

        CT_CHECK(arr.len() == 3);
        CT_CHECK(arr[0] == 1);
        CT_CHECK(arr[1] == 2);
        CT_CHECK(arr[2] == 3);
    }

    {
        struct TestStruct {
            i32 a;
            i32 b;
        };

        core::ArrList<TestStruct> arr;

        auto eqFn = [](const TestStruct& curr, addr_size, const TestStruct& el) -> bool { return curr.a == el.a; };

        core::pushUnique(arr, TestStruct{1, 2}, eqFn);
        core::pushUnique(arr, TestStruct{3, 4}, eqFn);
        TestStruct nonExistingLValue {5, 6};
        core::pushUnique(arr, nonExistingLValue, eqFn);
        TestStruct existingLValue {1, 2};
        core::pushUnique(arr, existingLValue, eqFn);

        CT_CHECK(arr.len() == 3);
        CT_CHECK(arr[0].a == 1);
        CT_CHECK(arr[0].b == 2);
        CT_CHECK(arr[1].a == 3);
        CT_CHECK(arr[1].b == 4);
        CT_CHECK(arr[2].a == 5);
        CT_CHECK(arr[2].b == 6);
    }

    {
        defer { CT::resetAll(); };

        CT ct; ct.a = 12;
        CT::resetAll(); // Disregard any ctors called by the code above.

        {
            core::ArrList<CT> arr(1);
            core::pushUnique(arr, std::move(ct), [](const CT& curr, addr_size, const CT& el) -> bool { return curr.a == el.a; });
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::moveCtorCalled() == 1);
            CT_CHECK(CT::dtorsCalled() == 0);

            CT::resetCtors();

            CT_CHECK(arr.len() == 1);
            CT_CHECK(arr[0].a == 12);
        }
        CT_CHECK(CT::dtorsCalled() == 1);
        CT_CHECK(ct.a == CT::defaultValue);
    }

    return 0;
}

constexpr i32 constFindAlgorithmTest() {
    {
        struct TestStruct {
            i32 a;
            i32 b;
        };

        core::ArrStatic<TestStruct, 3> staticArr;
        staticArr.push(TestStruct{1, 2});
        staticArr.push(TestStruct{3, 4});
        staticArr.push(TestStruct{5, 6});
        auto found = core::find(staticArr, [](const TestStruct& elem, addr_size) -> bool { return elem.a == 3; });
        CT_CHECK(found == 1);
        found = core::find(staticArr, [] (const TestStruct& elem, addr_size) -> bool { return elem.a == 6; });
        CT_CHECK(found == -1);
    }

    return 0;
}

constexpr i32 constBasicPushUniqueTest() {
    core::ArrStatic<i32, 5> staticArr;

    auto eqFn = [](const i32& curr, addr_size, const i32& el) -> bool { return curr == el; };

    core::pushUnique(staticArr, 1, eqFn);
    core::pushUnique(staticArr, 2, eqFn);
    i32 nonExistingLValue = 3;
    core::pushUnique(staticArr, nonExistingLValue, eqFn);
    i32 existingLValue = 1;
    core::pushUnique(staticArr, existingLValue, eqFn);

    CT_CHECK(staticArr.len() == 3);
    CT_CHECK(staticArr[0] == 1);
    CT_CHECK(staticArr[1] == 2);
    CT_CHECK(staticArr[2] == 3);

    return 0;
}

constexpr i32 quickSortTest() {
    struct TestCase {
        i32 input[20];
        i32 expected[20];
        addr_size len;
        i32 (*cmp)(i32 a, i32 b);
    };

    TestCase cases[] = {
        { { 2 }, { 2 }, 1, [](i32 a, i32 b) { return a - b; } },
        { { }, { }, 0, [](i32 a, i32 b) { return a - b; } },

        { { 2, 7, 3, 5, 9, 12, 9 }, { 2, 3, 5, 7, 9, 9, 12 }, 7, [](i32 a, i32 b) { return a - b; } },
        { { 2, 7, 3, 5, 9, 12, 9 }, { 12, 9, 9, 7, 5, 3, 2 }, 7, [](i32 a, i32 b) { return b - a; } },
        { { 1, 2, 3, 4, 5 },        { 1, 2, 3, 4, 5 },        5, [](i32 a, i32 b) { return a - b; } },
        { { 5, 4, 3, 2, 1 },        { 1, 2, 3, 4, 5 },        5, [](i32 a, i32 b) { return a - b; } },
        { { 7, 7, 7, 7 },           { 7, 7, 7, 7 },           4, [](i32 a, i32 b) { return a - b; } },
        { { -3, 0, 2, -1, 5, -3 },  { -3, -3, -1, 0, 2, 5 },  6, [](i32 a, i32 b) { return a - b; } },
        { { 3, 1, 2, 1, 3, 2, 1 },  { 1, 1, 1, 2, 2, 3, 3 },  7, [](i32 a, i32 b) { return a - b; } },

        {
            { core::limitMin<i32>(), 0, core::limitMax<i32>(), -1, 1 },
            { core::limitMin<i32>(), -1, 0, 1, core::limitMax<i32>() },
            5,
            [](i32 a, i32 b) { return (a > b) - (a < b); }
        },
    };

    i32 arr [] = { 2, 7, 3, 5, 9, 12, 9 };
    core::quickSort(arr, CORE_C_ARRLEN(arr), [](i32 a, i32 b) { return a - b; });

    i32 ret = core::testing::executeTestTable("quickSortTest test case failed at index: ", cases, [](TestCase& c, const char* cErr) {
        core::quickSort(c.input, c.len, c.cmp);
        bool equal = core::compareArraysBy(c.input, c.expected, c.len, [](i32 a, i32 b) { return a == b; });
        CT_CHECK(equal, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 runAlgorithmsTestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo(sInfo);
    tInfo.expectZeroAllocations = false;

    tInfo.name = FN_NAME_TO_CPTR(findAlgorithmTest);
    if (runTest(tInfo, findAlgorithmTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(forAllAlgorithmTest);
    if (runTest(tInfo, forAllAlgorithmTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(basicpushUniqueTest);
    if (runTest(tInfo, basicpushUniqueTest) != 0) { ret = -1; }

    // Below tests are expected to NOT allocate memory.
    tInfo.expectZeroAllocations = true;
    tInfo.allocatorName = nullptr;

    tInfo.name = FN_NAME_TO_CPTR(constFindAlgorithmTest);
    if (runTest(tInfo, constFindAlgorithmTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(constBasicPushUniqueTest);
    if (runTest(tInfo, constBasicPushUniqueTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(quickSortTest);
    if (runTest(tInfo, quickSortTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeAlgorithmTestsSuite() {
    RunTestCompileTime(constFindAlgorithmTest);
    RunTestCompileTime(constBasicPushUniqueTest);
    RunTestCompileTime(quickSortTest);

    return 0;
}

PRAGMA_WARNING_POP
