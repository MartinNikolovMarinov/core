#include "t-index.h"

constexpr i32 initializeStaticArrBasicTest() {
    using namespace core::testing;

    {
        core::ArrStatic<i32, 4> arr;
        CT_CHECK(arr.len() == 0);
        CT_CHECK(arr.cap() == 4);
        CT_CHECK(arr.empty());
    }

    {
        core::ArrStatic<i32, 5> arr(3, 99);
        CT_CHECK(arr.len() == 3);
        CT_CHECK(arr.byteLen() == 3 * sizeof(i32));
        CT_CHECK(arr.cap() == 5);
        CT_CHECK(arr.byteCap() == 5 * sizeof(i32));
        CT_CHECK(!arr.empty());
        for (addr_size i = 0; i < arr.len(); ++i) {
            CT_CHECK(arr[i] == 99);
        }
    }

    {
        auto arr = core::createArrStatic(2, 3, 4, 5, 6, 7, 8);
        CT_CHECK(arr.len() == 7);
        CT_CHECK(arr.cap() == 7);
        CT_CHECK(!arr.empty());
        for (addr_size i = 0; i < arr.len(); ++i) {
            CT_CHECK(arr[i] == i32(i + 2));
        }
    }

    {
        core::ArrStatic<i32, 3> a = core::createArrStatic(1, 2, 3);
        core::ArrStatic<i32, 3> b = core::createArrStatic(99, 99, 99);

        b = std::move(a);

        CT_CHECK(b.len() == 3);
        CT_CHECK(b[0] == 1 && a[0] == 1);
        CT_CHECK(b[1] == 2 && a[1] == 2);
        CT_CHECK(b[2] == 3 && a[2] == 3);
    }

    {
        core::ArrStatic<core::vec3f, 3> arr = core::createArrStatic(core::v(1.0f, 2.0f, 3.0f),
                                                                    core::v(4.0f, 5.0f, 6.0f),
                                                                    core::v(7.0f, 8.0f, 9.0f));

        CT_CHECK(arr.len() == 3);
        CT_CHECK(arr[0] == core::v(1.0f, 2.0f, 3.0f));
        CT_CHECK(arr[1] == core::v(4.0f, 5.0f, 6.0f));
        CT_CHECK(arr[2] == core::v(7.0f, 8.0f, 9.0f));
    }

    return 0;
}

constexpr i32 pushStaticArrTest() {
    {
        core::ArrStatic<i32, 4> arr;

        CT_CHECK(arr.cap() == 4);
        CT_CHECK(arr.len() == 0);
        CT_CHECK(arr.empty());

        arr.push(1);
        CT_CHECK(arr.len() == 1);
        arr.push(2);
        CT_CHECK(arr.len() == 2);
        arr.push(3);
        CT_CHECK(arr.len() == 3);
        arr.push(4);
        CT_CHECK(arr.len() == 4);

        CT_CHECK(!arr.empty());

        CT_CHECK(arr[0] == 1);
        CT_CHECK(arr.at(0) == 1);
        CT_CHECK(arr[1] == 2);
        CT_CHECK(arr.at(1) == 2);
        CT_CHECK(arr[2] == 3);
        CT_CHECK(arr.at(2) == 3);
        CT_CHECK(arr[3] == 4);
        CT_CHECK(arr.at(3) == 4);

        CT_CHECK(arr.first() == 1);
        CT_CHECK(arr.last() == 4);

        arr.clear();
        CT_CHECK(arr.len() == 0);
        CT_CHECK(arr.empty());

        arr.push(1);
        CT_CHECK(arr.len() == 1);
        arr.push(2);
        CT_CHECK(arr.len() == 2);
        arr.push(3);
        CT_CHECK(arr.len() == 3);
        arr.push(4);
        CT_CHECK(arr.len() == 4);
        CT_CHECK(!arr.empty());
    }

    {
        core::ArrStatic<char, 5> arr;
        char buff[] = {1, 2, 3};
        arr.push(buff, 3);

        CT_CHECK(arr.len() == 3);
        for (addr_size i = 0; i < arr.len(); ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
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

    CT_CHECK(arr1.len() == 4);
    CT_CHECK(arr1[0] == 5);
    CT_CHECK(arr1[1] == 6);
    CT_CHECK(arr1[2] == 7);
    CT_CHECK(arr1[3] == 8);

    core::ArrStatic<core::ArrStatic<i32, 4>, 4> arr3;

    arr3.push(arr1);
    arr3.push(arr2);

    CT_CHECK(arr3.len() == 2);

    CT_CHECK(arr3[0].len() == 4);
    CT_CHECK(arr3[0][0] == 5);
    CT_CHECK(arr3[0][1] == 6);
    CT_CHECK(arr3[0][2] == 7);
    CT_CHECK(arr3[0][3] == 8);

    CT_CHECK(arr3[1].len() == 4);
    CT_CHECK(arr3[1][0] == 5);
    CT_CHECK(arr3[1][1] == 6);
    CT_CHECK(arr3[1][2] == 7);
    CT_CHECK(arr3[1][3] == 8);

    return 0;
}

constexpr i32 runStaticArrRemoveTest() {
    core::ArrStatic<i32, 4> arr;
    arr.push(1);
    arr.push(2);
    arr.push(3);
    arr.push(4);

    CT_CHECK(arr.len() == 4);
    CT_CHECK(arr[0] == 1);
    CT_CHECK(arr[1] == 2);
    CT_CHECK(arr[2] == 3);
    CT_CHECK(arr[3] == 4);

    arr.remove(arr.len() - 1);

    CT_CHECK(arr.len() == 3);
    CT_CHECK(arr[0] == 1);
    CT_CHECK(arr[1] == 2);
    CT_CHECK(arr[2] == 3);

    arr.remove(1);

    CT_CHECK(arr.len() == 2);
    CT_CHECK(arr[0] == 1);
    CT_CHECK(arr[1] == 3);

    arr.remove(0);

    CT_CHECK(arr.len() == 1);
    CT_CHECK(arr[0] == 3);

    arr.remove(0);

    CT_CHECK(arr.len() == 0);

    return 0;
}

i32 assignStaticArrTest() {
    constexpr addr_size N = 15;
    core::ArrStatic<u8, 17> arr (N, 0);

    for (addr_size i = 0; i < N; i++) {
        arr.assign(u8(1), 0, i);
        for (addr_size j = 0; j < i; j++) {
            CT_CHECK(arr[j] == 1);
        }
        arr.replaceWith(0, N);

        arr.assign(u8(1), i, N);
        for (addr_size j = i; j < N; j++) {
            CT_CHECK(arr[j] == 1);
        }
        arr.replaceWith(0, N);
    }

    return 0;
}

i32 replaceWithStaticArrTest() {
    constexpr addr_size N = 15;
    core::ArrStatic<u8, N> arr (0, 0);

    arr.replaceWith(7, 8);
    for (addr_size i = 0; i < 8; i++) CT_CHECK(arr[i] == 7);

    arr.replaceWith(9, N);
    for (addr_size i = 0; i < N; i++) CT_CHECK(arr[i] == 9);

    arr.replaceWith(0, N);
    for (addr_size i = 0; i < N; i++) CT_CHECK(arr[i] == 0);

    arr.replaceWith(0, 0); // should not crash

    return 0;
}

i32 runStaticArrTestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo(sInfo);

    tInfo.name = FN_NAME_TO_CPTR(initializeStaticArrBasicTest);
    if (runTest(tInfo, initializeStaticArrBasicTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(pushStaticArrTest);
    if (runTest(tInfo, pushStaticArrTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(copyStaticArrTest);
    if (runTest(tInfo, copyStaticArrTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(runStaticArrRemoveTest);
    if (runTest(tInfo, runStaticArrRemoveTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(assignStaticArrTest);
    if (runTest(tInfo, assignStaticArrTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(replaceWithStaticArrTest);
    if (runTest(tInfo, replaceWithStaticArrTest) != 0) { return -1; }

    return 0;
}

constexpr i32 runCompletimeStaticArrTestsSuite() {
    RunTestCompileTime(initializeStaticArrBasicTest);
    RunTestCompileTime(pushStaticArrTest);
    RunTestCompileTime(copyStaticArrTest);
    RunTestCompileTime(runStaticArrRemoveTest);

    return 0;
}
