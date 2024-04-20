#include "t-index.h"

constexpr i32 initializeStaticArrBasicTest() {
    using namespace core::testing;

    {
        core::ArrStatic<i32, 4> arr;
        Assert(arr.len() == 0);
        Assert(arr.cap() == 4);
        Assert(arr.empty());
    }

    {
        core::ArrStatic<i32, 5> arr(3, 99);
        Assert(arr.len() == 3);
        Assert(arr.byteLen() == 3 * sizeof(i32));
        Assert(arr.cap() == 5);
        Assert(arr.byteCap() == 5 * sizeof(i32));
        Assert(!arr.empty());
        for (addr_size i = 0; i < arr.len(); ++i) {
            Assert(arr[i] == 99);
        }
    }

    {
        auto arr = core::createArrStatic(2, 3, 4, 5, 6, 7, 8);
        Assert(arr.len() == 7);
        Assert(arr.cap() == 7);
        Assert(!arr.empty());
        for (addr_size i = 0; i < arr.len(); ++i) {
            Assert(arr[i] == i32(i + 2));
        }
    }

    {
        core::ArrStatic<i32, 3> a;
        a.push(1); a.push(2); a.push(3);
        core::ArrStatic<i32, 3> b;
        b.push(4); b.push(5); b.push(6);

        b = std::move(a);

        Assert(b.len() == 3);
        Assert(b[0] == 1 && a[0] == 1);
        Assert(b[1] == 2 && a[1] == 2);
        Assert(b[2] == 3 && a[2] == 3);
    }

    return 0;
}

constexpr i32 pushStaticArrTest() {
    {
        core::ArrStatic<i32, 4> arr;

        Assert(arr.cap() == 4);
        Assert(arr.len() == 0);
        Assert(arr.empty());

        arr.push(1);
        Assert(arr.len() == 1);
        arr.push(2);
        Assert(arr.len() == 2);
        arr.push(3);
        Assert(arr.len() == 3);
        arr.push(4);
        Assert(arr.len() == 4);

        Assert(!arr.empty());

        Assert(arr[0] == 1);
        Assert(arr.at(0) == 1);
        Assert(arr[1] == 2);
        Assert(arr.at(1) == 2);
        Assert(arr[2] == 3);
        Assert(arr.at(2) == 3);
        Assert(arr[3] == 4);
        Assert(arr.at(3) == 4);

        Assert(arr.first() == 1);
        Assert(arr.last() == 4);

        arr.clear();
        Assert(arr.len() == 0);
        Assert(arr.empty());

        arr.push(1);
        Assert(arr.len() == 1);
        arr.push(2);
        Assert(arr.len() == 2);
        arr.push(3);
        Assert(arr.len() == 3);
        arr.push(4);
        Assert(arr.len() == 4);
        Assert(!arr.empty());
    }

    {
        core::ArrStatic<char, 5> arr;
        char buff[] = {1, 2, 3};
        arr.push(buff, 3);

        Assert(arr.len() == 3);
        for (addr_size i = 0; i < arr.len(); ++i) {
            Assert(arr[i] == i32(i + 1));
        }
    }

    return 0;
}

constexpr i32 copyStaticArrTest() {
    core::ArrStatic<i32, 4> arr1;
    arr1.push(1);
    arr1.push(2);
    arr1.push(3);
    arr1.push(4);

    core::ArrStatic<i32, 4> arr2;
    arr2.push(5);
    arr2.push(6);
    arr2.push(7);
    arr2.push(8);

    arr1 = arr2;

    Assert(arr1.len() == 4);
    Assert(arr1[0] == 5);
    Assert(arr1[1] == 6);
    Assert(arr1[2] == 7);
    Assert(arr1[3] == 8);

    core::ArrStatic<core::ArrStatic<i32, 4>, 4> arr3;

    arr3.push(arr1);
    arr3.push(arr2);

    Assert(arr3.len() == 2);

    Assert(arr3[0].len() == 4);
    Assert(arr3[0][0] == 5);
    Assert(arr3[0][1] == 6);
    Assert(arr3[0][2] == 7);
    Assert(arr3[0][3] == 8);

    Assert(arr3[1].len() == 4);
    Assert(arr3[1][0] == 5);
    Assert(arr3[1][1] == 6);
    Assert(arr3[1][2] == 7);
    Assert(arr3[1][3] == 8);

    return 0;
}

constexpr i32 runStaticArrRemoveTest() {
    core::ArrStatic<i32, 4> arr;
    arr.push(1);
    arr.push(2);
    arr.push(3);
    arr.push(4);

    Assert(arr.len() == 4);
    Assert(arr[0] == 1);
    Assert(arr[1] == 2);
    Assert(arr[2] == 3);
    Assert(arr[3] == 4);

    arr.remove(arr.len() - 1);

    Assert(arr.len() == 3);
    Assert(arr[0] == 1);
    Assert(arr[1] == 2);
    Assert(arr[2] == 3);

    arr.remove(1);

    Assert(arr.len() == 2);
    Assert(arr[0] == 1);
    Assert(arr[1] == 3);

    arr.remove(0);

    Assert(arr.len() == 1);
    Assert(arr[0] == 3);

    arr.remove(0);

    Assert(arr.len() == 0);

    return 0;
}

i32 runStaticArrTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo;
    tInfo.trackMemory = false;

    tInfo.name = FN_NAME_TO_CPTR(initializeStaticArrBasicTest);
    if (runTest(tInfo, initializeStaticArrBasicTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(pushStaticArrTest);
    if (runTest(tInfo, pushStaticArrTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(copyStaticArrTest);
    if (runTest(tInfo, copyStaticArrTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(runStaticArrRemoveTest);
    if (runTest(tInfo, runStaticArrRemoveTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompletimeStaticArrTestsSuite() {
    RunTestCompileTime(initializeStaticArrBasicTest);
    RunTestCompileTime(pushStaticArrTest);
    RunTestCompileTime(copyStaticArrTest);
    RunTestCompileTime(runStaticArrRemoveTest);

    return 0;
}

PRAGMA_WARNING_POP
