#include "t-index.h"

namespace {

template <typename T, core::AllocatorId TAllocId>
inline i32 checkEmptynessOfArr(const core::ArrList<T, TAllocId>& arr) {
    CT_CHECK(arr.len() == 0);
    CT_CHECK(arr.cap() == 0);
    CT_CHECK(arr.data() == nullptr);
    CT_CHECK(arr.empty());
    return 0;
}

template <core::AllocatorId TAllocId>
i32 initializeArrTest() {
    using namespace core::testing;

    {
        core::ArrList<i32, TAllocId> arr;
        CT_CHECK(checkEmptynessOfArr(arr) == 0);
    }

    {
        core::ArrList<i32, TAllocId> arr(10);
        CT_CHECK(arr.len() == 0);
        CT_CHECK(arr.cap() == 10);
        CT_CHECK(arr.empty());
    }

    {
        core::ArrList<i32, TAllocId> arr(10, 7);
        CT_CHECK(arr.len() == 10);
        CT_CHECK(arr.cap() == 10);
        CT_CHECK(!arr.empty());

        for (addr_size i = 0; i < arr.len(); ++i) {
            CT_CHECK(arr[i] == 7);
        }
    }

    {
        struct TestStruct {
            i32 a;
            f64 b;

            TestStruct() : a(7), b(8.0) {}
            TestStruct(const TestStruct& other) : a(other.a), b(other.b) {}
        };

        TestStruct v;
        core::ArrList<TestStruct, TAllocId> arr(10, v);

        CT_CHECK(arr.len() == 10);
        CT_CHECK(arr.byteLen() == 10 * sizeof(TestStruct));
        CT_CHECK(arr.cap() == 10);
        CT_CHECK(arr.byteCap() == 10 * sizeof(TestStruct));
        CT_CHECK(!arr.empty());

        for (addr_size i = 0; i < arr.len(); ++i) {
            CT_CHECK(arr[i].a == 7);
            CT_CHECK(arr[i].b == 8.0);
        }
    }

    // Move constructor and assignment
    {
        core::ArrList<i32, TAllocId> a(7, 1);
        core::ArrList<i32, TAllocId> b(std::move(a));

        CT_CHECK(a.len() == 0);

        CT_CHECK(b.len() == 7);
        CT_CHECK(b.cap() == 7);
        CT_CHECK(!b.empty());

        a = std::move(b);

        CT_CHECK(b.len() == 0);

        CT_CHECK(a.len() == 7);
        CT_CHECK(a.cap() == 7);
        CT_CHECK(!a.empty());

        PRAGMA_WARNING_PUSH

        DISABLE_GCC_AND_CLANG_WARNING(-Wself-move) // this is what I am testing here. I know it's wrong to do.

        a = std::move(a); // self assignment should not do anything

        CT_CHECK(a.len() == 7);
        CT_CHECK(a.cap() == 7);
        CT_CHECK(!a.empty());

        PRAGMA_WARNING_POP

        a = core::ArrList<i32, TAllocId>(3, 9); // should work and should not leak memory

        CT_CHECK(a.len() == 3);
        CT_CHECK(a.cap() == 3);
        CT_CHECK(!a.empty());
    }

    // Copy via the move constructor and assignment
    {
        core::ArrList<i32, TAllocId> a(7, 1);
        core::ArrList<i32, TAllocId> b(a.copy());

        CT_CHECK(a.len() == 7);
        CT_CHECK(a.cap() == 7);
        CT_CHECK(!a.empty());

        CT_CHECK(b.len() == 7);
        CT_CHECK(b.cap() == 7);
        CT_CHECK(!b.empty());

        CT_CHECK(b.data() != a.data());

        core::ArrList<i32, TAllocId> c;
        c = a.copy();

        CT_CHECK(c.len() == 7);
        CT_CHECK(c.cap() == 7);
        CT_CHECK(!c.empty());

        CT_CHECK(c.data() != a.data());
    }

    {
        defer { CT::resetAll(); };
        constexpr i32 testCount = 10;

        CT x = {};
        CT::resetCtors(); // Don't count constructor calls for x

        {
            core::ArrList<CT, TAllocId> arr(testCount, x);
            for (addr_size i = 0; i < arr.len(); ++i) {
                CT_CHECK(arr[i].a == 7);
            }
            CT_CHECK(CT::totalCtorsCalled() == testCount);
            CT_CHECK(CT::copyCtorCalled() == testCount);
            CT::resetCtors();

            {
                auto arrCpy = arr.copy();
                for (addr_size i = 0; i < arrCpy.len(); ++i) {
                    CT_CHECK(arrCpy[i].a == 7);
                }
                CT_CHECK(CT::totalCtorsCalled() == testCount);
                CT_CHECK(CT::copyCtorCalled() == testCount);
                CT::resetCtors();
            }
            CT_CHECK(CT::dtorsCalled() == testCount);
            CT::resetDtors();
        }
        CT_CHECK(CT::dtorsCalled() == testCount);
        CT::resetDtors();
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 ensureArrCapacity() {
    core::ArrList<i32, TAllocId> arr;
    CT_CHECK(checkEmptynessOfArr(arr) == 0);

    arr.ensureCap(10);
    CT_CHECK(arr.len() == 0);
    CT_CHECK(arr.cap() == 10);
    CT_CHECK(arr.data() != nullptr);
    CT_CHECK(arr.empty());

    arr.ensureCap(2);
    CT_CHECK(arr.len() == 0);
    CT_CHECK(arr.cap() == 10);
    CT_CHECK(arr.data() != nullptr);
    CT_CHECK(arr.empty());

    arr.ensureCap(11);
    CT_CHECK(arr.len() == 0);
    CT_CHECK(arr.cap() == 11);
    CT_CHECK(arr.data() != nullptr);
    CT_CHECK(arr.empty());

    return 0;
}

template <core::AllocatorId TAllocId>
i32 pushIntoArrTests() {
    using CT = core::testing::CT;

    {
        core::ArrList<i32, TAllocId> arr;

        arr.push(1);
        for (addr_size i = 0; i < 1; ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
            CT_CHECK(arr.at(i) == i32(i + 1));
        }

        arr.push(2);
        for (addr_size i = 0; i < 2; ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
            CT_CHECK(arr.at(i) == i32(i + 1));
        }

        arr.clear();

        arr.push(1);
        for (addr_size i = 0; i < 1; ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
            CT_CHECK(arr.at(i) == i32(i + 1));
        }

        arr.push(2);
        for (addr_size i = 0; i < 2; ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
            CT_CHECK(arr.at(i) == i32(i + 1));
        }

        arr.push(3);
        CT_CHECK(arr.len() == 3);
        CT_CHECK(arr.cap() >= arr.len());
        for (addr_size i = 0; i < 3; ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
            CT_CHECK(arr.at(i) == i32(i + 1));
        }

        arr.clear();

        // Append many trivial values.
        i32 many[5] = { 1, 2, 3, 4, 5 };
        arr.push(many, 5);
        CT_CHECK(arr.len() == 5);
        CT_CHECK(arr.cap() >= arr.len());
        for (addr_size i = 0; i < 5; ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
            CT_CHECK(arr.at(i) == i32(i + 1));
        }
    }

    {
        defer { CT::resetAll(); };
        CT lv;
        CT lv2;
        CT::resetCtors(); // Don't count the default ctors of the above code

        {
            core::ArrList<CT, TAllocId> arr;
            arr.push(lv);
            arr.push(std::move(lv2));
            CT_CHECK(CT::copyCtorCalled() == 1, "push should call the copy ctor.");
            CT_CHECK(CT::moveCtorCalled() == 1, "push should call the move ctor.");
            CT_CHECK(CT::assignmentsCopyCalled() == 0, "push should NOT call the copy assignment.");
            CT_CHECK(CT::assignmentsMoveCalled() == 0, "push should NOT call the move assignment.");
            for (addr_size i = 0; i < arr.len(); ++i) {
                CT_CHECK(arr[i].a == 7, "The array default values are not correct.");
            }
        }
        CT_CHECK(CT::dtorsCalled() == 2);
        CT::resetAll();

        // Testing a combination of append and ensureCap.
        {
            core::ArrList<CT, TAllocId> arr;
            arr.ensureCap(1);
            CT_CHECK(arr.len() == 0);
            CT_CHECK(arr.cap() == 1);
            CT_CHECK(CT::noCtorsCalled());

            arr.push(std::move(lv2));
            arr.push(lv);
            for (addr_size i = 0; i < arr.len(); ++i) {
                CT_CHECK(arr[i].a == 7, "The array default values are not correct.");
            }
            CT_CHECK(arr.len() == 2);
            CT_CHECK(arr.cap() == 2);
            CT_CHECK(CT::copyCtorCalled() == 1);
            CT_CHECK(CT::moveCtorCalled() == 1);

            arr.free(); // This should call the destructors.
            CT_CHECK(arr.len() == 0);
            CT_CHECK(arr.cap() == 0);
            CT_CHECK(CT::dtorsCalled() == 2);
        }
        CT_CHECK(CT::dtorsCalled() == 2);
        CT::resetAll();

        // Test appending multiple values
        CT staticArr[5];
        CT::resetCtors(); // Don't count the default ctors of the above code
        {
            core::ArrList<CT, TAllocId> arr;
            arr.push(staticArr, 5);

            CT_CHECK(arr.len() == 5);
            CT_CHECK(arr.cap() >= arr.len());

            CT_CHECK(CT::copyCtorCalled() == 5);
            CT_CHECK(CT::moveCtorCalled() == 0);
            CT_CHECK(CT::assignmentsCopyCalled() == 0);
            CT_CHECK(CT::assignmentsMoveCalled() == 0);
            CT_CHECK(CT::defaultCtorCalled() == 0);

            for (addr_size i = 0; i < arr.len(); ++i) {
                CT_CHECK(arr[i].a == 7, "The array default values are not correct.");
            }
        }
        CT_CHECK(CT::dtorsCalled() == 5);
        CT::resetAll();
    }

    {
        // Appending arrays that are bigger than double current capacity.
        core::ArrList<i32, TAllocId> arr;
        core::ArrList<i32, TAllocId> arr2;
        arr2.push(1); arr2.push(2); arr2.push(3); arr2.push(4); arr2.push(5);
        arr2.push(6); arr2.push(7); arr2.push(8); arr2.push(9); arr2.push(10);

        arr.push(arr2.data(), arr2.len());

        CT_CHECK(arr.len() == 10);
        CT_CHECK(arr.cap() >= arr.len());
        for (addr_size i = 0; i < 10; ++i) {
            CT_CHECK(arr[i] == i32(i + 1));
            CT_CHECK(arr.at(i) == i32(i + 1));
        }
    }

    {
        core::ArrList<i32, TAllocId> arr (15, 999);
        i32 arr2[5] = { 1, 2, 3, 4, 5 };

        arr.push(arr2, 5);

        CT_CHECK(arr.len() == 20);
        CT_CHECK(arr.cap() >= arr.len());
        for (addr_size i = 0; i < 15; ++i) {
            CT_CHECK(arr[i] == 999);
        }
        for (addr_size i = 0; i < 5; ++i) {
            CT_CHECK(arr[i + 15] == i32(i + 1));
        }
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 arrayOfArraysArrTest() {
    {
        core::ArrList<i32, TAllocId> arr;
        core::ArrList<i32, TAllocId> arr2;
        core::ArrList<i32, TAllocId> arr3;
        core::ArrList<core::ArrList<i32, TAllocId>> multi;

        arr.push(1);
        arr.push(2);
        arr.push(3);

        arr2.push(4);
        arr2.push(5);
        arr2.push(6);

        arr3.push(7);
        arr3.push(8);
        arr3.push(9);

        multi.push(arr.copy());
        multi.push(std::move(arr2));
        multi.push(std::move(arr3));

        // arr 1 should be copied
        CT_CHECK(arr.len() == 3);
        CT_CHECK(arr[0] == 1);
        CT_CHECK(arr[1] == 2);
        CT_CHECK(arr[2] == 3);

        // arr 2 and 3 should have been moved
        CT_CHECK(arr2.len() == 0);
        CT_CHECK(arr2.data() == nullptr);
        CT_CHECK(arr3.len() == 0);
        CT_CHECK(arr3.data() == nullptr);

        CT_CHECK(multi.len() == 3);
        CT_CHECK(multi[0].len() == 3);
        CT_CHECK(multi[1].len() == 3);
        CT_CHECK(multi[2].len() == 3);
        CT_CHECK(multi[0][0] == 1);
        CT_CHECK(multi[0][1] == 2);
        CT_CHECK(multi[0][2] == 3);
        CT_CHECK(multi[1][0] == 4);
        CT_CHECK(multi[1][1] == 5);
        CT_CHECK(multi[1][2] == 6);
        CT_CHECK(multi[2][0] == 7);
        CT_CHECK(multi[2][1] == 8);
        CT_CHECK(multi[2][2] == 9);
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 clearArrShouldCallDtorsTest() {
    using CT = core::testing::CT;

    defer { CT::resetAll(); };

    CT x;
    CT::resetAll(); // Don't count the default ctors for x

    constexpr i32 testCount = 10;
    auto arr = core::ArrList<CT, TAllocId>(testCount, x);
    arr.clear();
    CT_CHECK(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
    CT_CHECK(arr.cap() == testCount, "Clear should not change the capacity of the array.");

    CT::resetDtors();

    arr.clear();
    arr = core::ArrList<CT, TAllocId>(testCount, x);
    arr.clear();
    CT_CHECK(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
    CT_CHECK(arr.cap() == testCount, "Clear should not change the capacity of the array.");

    return 0;
}

template <core::AllocatorId TAllocId>
i32 removeFromArrTest() {
    using CT = core::testing::CT;

    {
        core::ArrList<i32, TAllocId> arr;

        {
            arr.push(1);
            arr.remove(0);
            CT_CHECK(arr.len() == 0);
            arr.clear();
        }
        {
            arr.push(1);
            arr.push(2);
            arr.remove(arr.len() - 1);
            CT_CHECK(arr.len() == 1);
            CT_CHECK(arr[0] == 1);
            arr.clear();
        }
        {
            arr.push(1);
            arr.push(2);
            arr.remove(0);
            CT_CHECK(arr.len() == 1);
            CT_CHECK(arr[0] == 2);
            arr.clear();
        }
    }

    {
        core::ArrList<CT, TAllocId> arr;

        defer { CT::resetAll(); };

        CT a, b;

        {
            CT::resetDtors();
            arr.push(a);
            arr.remove(0);
            CT_CHECK(CT::dtorsCalled() == 1);
            CT_CHECK(arr.len() == 0);
            arr.clear();
        }
        {
            CT::resetDtors();
            arr.push(a);
            arr.push(b);
            arr.remove(arr.len() - 1);
            CT_CHECK(CT::dtorsCalled() == 1);
            CT_CHECK(arr.len() == 1);
            arr.clear();
        }
        {
            CT::resetDtors();
            arr.push(a);
            arr.push(b);
            arr.remove(0);
            CT_CHECK(CT::dtorsCalled() == 1);
            CT_CHECK(arr.len() == 1);
            arr.clear();
        }
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 resetArrTest() {
    constexpr i32 ALLOCATION_SIZE = 256;
    auto unmanaged = reinterpret_cast<u8*>(core::getAllocator(TAllocId).zeroAlloc(ALLOCATION_SIZE, sizeof(u8)));
    core::memset(unmanaged, u8(5), ALLOCATION_SIZE);

    core::ArrList<u8, TAllocId> arr (5, 10); // the previously allocated also needs to be freed.
    arr.reset(&unmanaged, ALLOCATION_SIZE, ALLOCATION_SIZE); // arr becomes the owner and thus must free the memory when it goes out of scope.

    CT_CHECK(arr.len() == ALLOCATION_SIZE);
    CT_CHECK(arr.cap() == ALLOCATION_SIZE);
    CT_CHECK(arr.data() != nullptr);
    CT_CHECK(unmanaged == nullptr);

    for (addr_size i = 0; i < arr.len(); ++i) {
        CT_CHECK(arr[i] == 5);
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 releaseArrTest() {
    core::ArrList<u8, TAllocId> arr (15, 123);

    addr_size l = 0, c = 0;
    auto data = arr.release(l, c);

    CT_CHECK(checkEmptynessOfArr(arr) == 0, "Array should be empty after release.");

    for (addr_size i = 0; i < l; ++i) {
        CT_CHECK(data[i] == 123);
    }

    // There should be a memory leak if the released data is not freed!
    core::getAllocator(TAllocId).free(data, c, sizeof(u8));

    return 0;
}

template <core::AllocatorId TAllocId>
i32 assignArrTest() {
    constexpr addr_size N = 15;
    core::ArrList<u8, TAllocId> arr (N, 0);

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

template <core::AllocatorId TAllocId>
i32 replaceWithArrTest() {
    constexpr addr_size N = 15;
    core::ArrList<u8, TAllocId> arr;

    arr.replaceWith(7, 8);
    for (addr_size i = 0; i < 8; i++) CT_CHECK(arr[i] == 7);

    arr.replaceWith(9, N);
    for (addr_size i = 0; i < N; i++) CT_CHECK(arr[i] == 9);

    arr.replaceWith(0, N);
    for (addr_size i = 0; i < N; i++) CT_CHECK(arr[i] == 0);

    arr.replaceWith(0, 0); // should not crash

    return 0;
}

template <core::AllocatorId TAllocId>
i32 convArrTest() {
    // First Test: Reinterpret u8 to char
    {
        constexpr addr_size N = 17;
        core::ArrList<u8, TAllocId> arr1(N, u8('a'));
        core::ArrList<char, TAllocId> arr2;
        core::convArrList(arr2, std::move(arr1));

        CT_CHECK(arr1.len() == 0);
        CT_CHECK(arr1.cap() == 0);
        CT_CHECK(arr1.data() == nullptr);

        for (addr_size i = 0; i < N; i++) {
            CT_CHECK(i32('a') == i32(arr2[i]));
        }
    }

    // Second Test: Reinterpret u8 to u16
    {
        constexpr addr_size N = 10; // Total bytes = N * sizeof(u8)
        core::ArrList<u8, TAllocId> arr1(N * 2, 0xAB);
        core::ArrList<u16, TAllocId> arr2;
        core::convArrList(arr2, std::move(arr1));

        CT_CHECK(arr1.len() == 0);
        CT_CHECK(arr1.cap() == 0);
        CT_CHECK(arr1.data() == nullptr);

        constexpr u16 expectedValue = 0xABAB;
        for (addr_size i = 0; i < N; i++) {
            CT_CHECK(arr2[i] == expectedValue);
        }
    }

    // Third Test: Reinterpret struct of u8 and u32 to u8
    {
        struct TestStruct {
            u8 byteVal;
            u32 intVal;
        };

        constexpr addr_size N = 1; // Total structs
        core::ArrList<TestStruct, TAllocId> structArr(N, {0xAB, 0x12345678});
        core::ArrList<u8, TAllocId> byteArr;
        core::convArrList(byteArr, std::move(structArr));

        CT_CHECK(structArr.len() == 0);
        CT_CHECK(structArr.cap() == 0);
        CT_CHECK(structArr.data() == nullptr);

        constexpr addr_size structSize = sizeof(TestStruct);
        CT_CHECK(byteArr.len() == N * structSize);

        for (addr_size i = 0; i < N; i++) {
            addr_size offset = i * structSize;

            // Check the u8 value
            CT_CHECK(byteArr[offset] == 0xAB);

            // Calculate the offset for the u32 value based on alignment
            constexpr addr_size intOffset = offsetof(TestStruct, intVal);
            u32 intValue = *reinterpret_cast<u32*>(byteArr.data() + offset + intOffset);

            // Check the u32 value
            CT_CHECK(intValue == 0x12345678);
        }
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 runTests() {
    using namespace core::testing;

    TestInfo tInfo = createTestInfoFor(RegisteredAllocators(TAllocId));

    defer { core::getAllocator(TAllocId).clear(); };

    tInfo.name = FN_NAME_TO_CPTR(initializeArrTest);
    if (runTest(tInfo, initializeArrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(ensureArrCapacity);
    if (runTest(tInfo, ensureArrCapacity<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(pushIntoArrTests);
    if (runTest(tInfo, pushIntoArrTests<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(arrayOfArraysArrTest);
    if (runTest(tInfo, arrayOfArraysArrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(clearArrShouldCallDtorsTest);
    if (runTest(tInfo, clearArrShouldCallDtorsTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(removeFromArrTest);
    if (runTest(tInfo, removeFromArrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(resetArrTest);
    if (runTest(tInfo, resetArrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(releaseArrTest);
    if (runTest(tInfo, releaseArrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(assignArrTest);
    if (runTest(tInfo, assignArrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(replaceWithArrTest<TAllocId>);
    if (runTest(tInfo, replaceWithArrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(convArrTest<TAllocId>);
    if (runTest(tInfo, convArrTest<TAllocId>) != 0) { return -1; }

    return 0;
}

} // namespace

i32 runArrTestsSuite() {
    using namespace core::testing;

    if (runTests<RA_STD_ALLOCATOR_ID>() != 0) { return -1; }
    if (runTests<RA_STD_STATS_ALLOCATOR_ID>() != 0) { return -1; }
    if (runTests<RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID>() != 0) { return -1; }
    if (runTests<RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID>() != 0) { return -1; }

    constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE * 3;
    char buf[BUFFER_SIZE];
    setBufferForBumpAllocator(buf, BUFFER_SIZE);
    if (runTests<RA_BUMP_ALLOCATOR_ID>() != 0) { return -1; }

    setBlockSizeForArenaAllocator(256);
    if (runTests<RA_ARENA_ALLOCATOR_ID>() != 0) { return -1; }

    return 0;
}
