#include "t-index.h"

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

i32 runAlgorithmsTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(findAlgorithmTest);
    if (runTest(tInfo, findAlgorithmTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(basicpushUniqueTest);
    if (runTest(tInfo, basicpushUniqueTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(constFindAlgorithmTest);
    if (runTest(tInfo, constFindAlgorithmTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(constBasicPushUniqueTest);
    if (runTest(tInfo, constBasicPushUniqueTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeAlgorithmTestsSuite() {
    RunTestCompileTime(constFindAlgorithmTest);
    RunTestCompileTime(constBasicPushUniqueTest);

    return 0;
}

PRAGMA_WARNING_POP
