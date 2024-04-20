#include "t-index.h"

PRAGMA_WARNING_PUSH

DISABLE_MSVC_WARNING(4127) // Conditional expression is constant. I don't care here.

i32 findAlgorithmTest() {
    {
        constexpr addr_size BUFF_LEN = 5;
        i32 arr[BUFF_LEN] = {1, 2, 3, 4, 5};
        addr_off found = core::find(arr, BUFF_LEN, [](i32 elem, addr_off) -> bool { return elem == 3; });
        Assert(found == 2);
        found = core::find(arr, BUFF_LEN, [] (i32 elem, addr_off) -> bool { return elem == 6; });
        Assert(found == -1);
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
        addr_off found = core::find(arr, [](const TestStruct& elem, addr_off) -> bool { return elem.a == 3; });
        Assert(found == 1);
        found = core::find(arr, [] (const TestStruct& elem, addr_off) -> bool { return elem.a == 6; });
        Assert(found == -1);
    }

    return 0;
}

i32 basicpushUniqueTest() {
    {
        core::ArrList<i32> arr;

        auto eqFn = [](const i32& curr, addr_off, const i32& el) -> bool { return curr == el; };

        core::pushUnique(arr, 1, eqFn);
        core::pushUnique(arr, 2, eqFn);
        i32 nonExistingLValue = 3;
        core::pushUnique(arr, nonExistingLValue, eqFn);
        i32 existingLValue = 1;
        core::pushUnique(arr, existingLValue, eqFn);

        Assert(arr.len() == 3);
        Assert(arr[0] == 1);
        Assert(arr[1] == 2);
        Assert(arr[2] == 3);
    }

    {
        struct TestStruct {
            i32 a;
            i32 b;
        };

        core::ArrList<TestStruct> arr;

        auto eqFn = [](const TestStruct& curr, addr_off, const TestStruct& el) -> bool { return curr.a == el.a; };

        core::pushUnique(arr, TestStruct{1, 2}, eqFn);
        core::pushUnique(arr, TestStruct{3, 4}, eqFn);
        TestStruct nonExistingLValue {5, 6};
        core::pushUnique(arr, nonExistingLValue, eqFn);
        TestStruct existingLValue {1, 2};
        core::pushUnique(arr, existingLValue, eqFn);

        Assert(arr.len() == 3);
        Assert(arr[0].a == 1);
        Assert(arr[0].b == 2);
        Assert(arr[1].a == 3);
        Assert(arr[1].b == 4);
        Assert(arr[2].a == 5);
        Assert(arr[2].b == 6);
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
        auto found = core::find(staticArr, [](const TestStruct& elem, addr_off) -> bool { return elem.a == 3; });
        Assert(found == 1);
        found = core::find(staticArr, [] (const TestStruct& elem, addr_off) -> bool { return elem.a == 6; });
        Assert(found == -1);
    }

    return 0;
}

constexpr i32 constBasicpushUniqueTest() {
    core::ArrStatic<i32, 5> staticArr;

    auto eqFn = [](const i32& curr, addr_off, const i32& el) -> bool { return curr == el; };

    core::pushUnique(staticArr, 1, eqFn);
    core::pushUnique(staticArr, 2, eqFn);
    i32 nonExistingLValue = 3;
    core::pushUnique(staticArr, nonExistingLValue, eqFn);
    i32 existingLValue = 1;
    core::pushUnique(staticArr, existingLValue, eqFn);

    Assert(staticArr.len() == 3);
    Assert(staticArr[0] == 1);
    Assert(staticArr[1] == 2);
    Assert(staticArr[2] == 3);

    return 0;
}

i32 runAlgorithmsTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo;
    tInfo.trackMemory = false;

    tInfo.name = FN_NAME_TO_CPTR(findAlgorithmTest);
    if (runTest(tInfo, findAlgorithmTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(basicpushUniqueTest);
    if (runTest(tInfo, basicpushUniqueTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(constFindAlgorithmTest);
    if (runTest(tInfo, constFindAlgorithmTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(constBasicpushUniqueTest);
    if (runTest(tInfo, constBasicpushUniqueTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeAlgorithmTestsSuite() {
    RunTestCompileTime(constFindAlgorithmTest);
    RunTestCompileTime(constBasicpushUniqueTest);

    return 0;
}

PRAGMA_WARNING_POP
