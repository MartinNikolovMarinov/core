#include "t-index.h"

i32 initializeArrTest() {
    {
        core::ArrList<i32> arr;
        CT_CHECK(arr.len() == 0);
        CT_CHECK(arr.cap() == 0);
        CT_CHECK(arr.data() == nullptr);
        CT_CHECK(arr.empty());
    }

    {
        core::ArrList<i32> arr(10);
        CT_CHECK(arr.len() == 0);
        CT_CHECK(arr.cap() == 10);
        CT_CHECK(arr.empty());
    }

    {
        core::ArrList<i32> arr(10, 7);
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
        core::ArrList<TestStruct> arr(10, v);

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
        core::ArrList<i32> a(7, 1);
        core::ArrList<i32> b(std::move(a));

        CT_CHECK(a.len() == 0);
        CT_CHECK(a.cap() == 0);
        CT_CHECK(a.data() == nullptr);
        CT_CHECK(a.empty());

        CT_CHECK(b.len() == 7);
        CT_CHECK(b.cap() == 7);
        CT_CHECK(!b.empty());

        a = std::move(b);

        CT_CHECK(b.len() == 0);
        CT_CHECK(b.cap() == 0);
        CT_CHECK(b.data() == nullptr);
        CT_CHECK(b.empty());

        CT_CHECK(a.len() == 7);
        CT_CHECK(a.cap() == 7);
        CT_CHECK(!a.empty());

        a = std::move(a); // self assignment should not do anything

        CT_CHECK(a.len() == 7);
        CT_CHECK(a.cap() == 7);
        CT_CHECK(!a.empty());

        a = core::ArrList<i32>(3, 9); // should work and should not leak memory

        CT_CHECK(a.len() == 3);
        CT_CHECK(a.cap() == 3);
        CT_CHECK(!a.empty());
    }

    // Copy via the move constructor and assignment
    {
        core::ArrList<i32> a(7, 1);
        core::ArrList<i32> b(a.copy());

        CT_CHECK(a.len() == 7);
        CT_CHECK(a.cap() == 7);
        CT_CHECK(!a.empty());

        CT_CHECK(b.len() == 7);
        CT_CHECK(b.cap() == 7);
        CT_CHECK(!b.empty());

        CT_CHECK(b.data() != a.data());

        core::ArrList<i32> c;
        c = a.copy();

        CT_CHECK(c.len() == 7);
        CT_CHECK(c.cap() == 7);
        CT_CHECK(!c.empty());

        CT_CHECK(c.data() != a.data());
    }

    {
        defer { core::testing::CT::resetAll(); };
        constexpr i32 testCount = 10;

        core::testing::CT x = {};
        core::testing::CT::resetCtors(); // Don't count constructor calls for x

        {
            core::ArrList<core::testing::CT> arr(testCount, x);
            for (addr_size i = 0; i < arr.len(); ++i) {
                CT_CHECK(arr[i].a == 7);
            }
            CT_CHECK(core::testing::CT::totalCtorsCalled() == testCount);
            CT_CHECK(core::testing::CT::copyCtorCalled() == testCount);
            core::testing::CT::resetCtors();

            {
                auto arrCpy = arr.copy();
                for (addr_size i = 0; i < arrCpy.len(); ++i) {
                    CT_CHECK(arrCpy[i].a == 7);
                }
                CT_CHECK(core::testing::CT::totalCtorsCalled() == testCount);
                CT_CHECK(core::testing::CT::copyCtorCalled() == testCount);
                core::testing::CT::resetCtors();
            }
            CT_CHECK(core::testing::CT::dtorsCalled() == testCount);
            core::testing::CT::resetDtors();
        }
        CT_CHECK(core::testing::CT::dtorsCalled() == testCount);
        core::testing::CT::resetDtors();
    }

    return 0;
}

i32 ensureArrCapacity() {
    core::ArrList<i32> arr;
    CT_CHECK(arr.len() == 0);
    CT_CHECK(arr.cap() == 0);
    CT_CHECK(arr.data() == nullptr);
    CT_CHECK(arr.empty());

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

// template <typename TAllocator>
// i32 fillArrTest() {

//     // Invalid from and to values should not have any effect.
//     {
//         core::Arr<i32, TAllocator> arr;

//         arr.fill(0, 1, 0);
//         CT_CHECK(arr.len() == 0);
//         CT_CHECK(arr.cap() == 0);
//         CT_CHECK(arr.data() == nullptr);
//         CT_CHECK(arr.empty());

//         arr.fill(0, 0, 0);
//         CT_CHECK(arr.len() == 0);
//         CT_CHECK(arr.cap() == 0);
//         CT_CHECK(arr.data() == nullptr);
//         CT_CHECK(arr.empty());
//     }

//     // Filling an array exactly to capacity:
//     {
//         constexpr i32 N = 10;
//         core::Arr<i32, TAllocator> arr(N);

//         arr.fill(0, 0, N);
//         for (addr_size i = 0; i < N; ++i) {
//             CT_CHECK(arr[i] == 0);
//         }

//         arr.fill(1, 0, N);
//         for (addr_size i = 0; i < N; ++i) {
//             CT_CHECK(arr[i] == 1);
//         }
//     }

//     // Filling an array of pointers exactly to capacity with a structure:
//     {
//         constexpr i32 N = 10;
//         struct TestStruct {
//             i32 a;
//             f64 b;
//         };
//         core::Arr<TestStruct*, TAllocator> arr2(N);
//         TestStruct t = { 1, 2.0 };

//         arr2.fill(&t, 0, N);
//         for (addr_size i = 0; i < arr2.len(); ++i) {
//             CT_CHECK(arr2[i]->a == 1);
//             CT_CHECK(arr2[i]->b == 2.0);
//         }

//         arr2.fill(nullptr, 0, N);
//         for (addr_size i = 0; i < arr2.len(); ++i) {
//             CT_CHECK(arr2[i] == nullptr);
//         }
//     }

//     // Fill emtpy array:
//     {
//         core::Arr<i32, TAllocator> arr;

//         arr.fill(0, 0, 10);
//         CT_CHECK(arr.len() == 10);
//         CT_CHECK(arr.cap() >= arr.len());
//         for (addr_size i = 0; i < arr.len(); ++i) {
//             CT_CHECK(arr[i] == 0);
//         }
//     }

//     // Partial fill:
//     {
//         core::Arr<i32, TAllocator> arr (10);
//         arr.fill(0, 0, 10);

//         arr.fill(1, 0, 5);

//         for (addr_size i = 0; i < 5; ++i) {
//             CT_CHECK(arr[i] == 1);
//         }
//         for (addr_size i = 5; i < 10; ++i) {
//             CT_CHECK(arr[i] == 0);
//         }
//     }

//     // Assure proper object destructors are called:
//     {
//         using CT = core::testing::CT;

//         CT::resetAll();
//         defer { CT::resetAll(); };
//         constexpr i32 testCount = 10;
//         CT v;

//         {
//             core::Arr<CT, TAllocator> arr;
//             arr.fill(v, 0, testCount);
//             for (addr_size i = 0; i < arr.len(); ++i) {
//                 CT_CHECK(arr[i].a == CT::defaultValue, "Fill did not call the default constructors.");
//                 arr[i].a = 8; // Modify the default should not affect the original object v.
//             }
//             CT_CHECK(CT::dtorsCalled() == 0, "No destructors should be called by fill in this case.");
//             CT_CHECK(v.a == 7, "The value passed to the fill function was modified.");
//         }
//         CT_CHECK(CT::dtorsCalled() == testCount, "Destructors where not called after the array went out of scope.");
//         CT::resetAll();

//         {
//             v.a = 9; // Fill must use this value!

//             core::Arr<CT, TAllocator> arr(testCount); // Initially all elements are default constructed.

//             arr.fill(v, 0, testCount / 2); // Filling half to CT_CHECK that the ones that are overwritten are freed.

//             for (addr_size i = 0; i < testCount / 2; ++i) {
//                 CT_CHECK(arr[i].a == 9, "Fill did not work.");
//             }
//             for (addr_size i = testCount / 2; i < arr.len(); ++i) {
//                 CT_CHECK(arr[i].a == CT::defaultValue, "Fill overwrote elements that it should not have.");
//             }
//             CT_CHECK(CT::dtorsCalled() == testCount / 2, "Exactly half of the destructors should have been called by fill.");

//             arr.fill(v, testCount / 2, testCount); // Filling the rest of the array should call the remaining destructors.

//             for (addr_size i = 0; i < arr.len(); ++i) {
//                 CT_CHECK(arr[i].a == 9, "Fill did not work.");
//             }
//             CT_CHECK(CT::dtorsCalled() == testCount, "All destructors should have been called by fill.");

//             CT::resetDtors();
//         }
//         CT_CHECK(CT::dtorsCalled() == testCount, "Destructors where not called after the array went out of scope.");
//         CT::resetAll();

//         {
//             v.a = 7; // Fill must use this value!

//             core::Arr<CT, TAllocator> arr(testCount / 2); // Initially half of the elements are default constructed.

//             arr.fill(v, 0, testCount); // Filling the whole array should call only half of the destructors.

//             for (addr_size i = 0; i < arr.len(); ++i) {
//                 CT_CHECK(arr[i].a == 7, "Fill did not work.");
//             }
//             CT_CHECK(CT::dtorsCalled() == testCount / 2, "Exactly half of the destructors should have been called by fill.");

//             CT::resetDtors();
//         }
//         CT_CHECK(CT::dtorsCalled() == testCount, "Destructors where not called after the array went out of scope.");
//         CT::resetAll();

//         {
//             v.a = 200; // Fill must use this value!

//             core::Arr<CT, TAllocator> arr(testCount); // Initially all elements are default constructed.

//             arr.fill(v, 2, 5); // Filling a range of the array should call only the destructors of the overwritten elements.

//             for (addr_size i = 0; i < arr.len(); ++i) {
//                 if (i >= 2 && i < 5) {
//                     CT_CHECK(arr[i].a == 200, "Fill did not work.");
//                 }
//                 else {
//                     CT_CHECK(arr[i].a == CT::defaultValue, "Fill overwrote elements that it should not have.");
//                 }
//             }
//             CT_CHECK(CT::dtorsCalled() == 3, "Exactly 3 destructors should have been called by fill.");

//             CT::resetDtors();
//         }
//         CT_CHECK(CT::dtorsCalled() == testCount, "Destructors where not called after the array went out of scope.");
//         CT::resetAll();
//     }

//     return 0;
// }

// template <typename TAllocator>
// i32 appendArrTest() {
//     using CT = core::testing::CT;

//     {
//         core::Arr<i32, TAllocator> arr;

//         arr.append(1);
//         for (addr_size i = 0; i < 1; ++i) {
//             CT_CHECK(arr[i] == i32(i + 1));
//             CT_CHECK(arr.at(i) == i32(i + 1));
//         }

//         arr.append(2);
//         for (addr_size i = 0; i < 2; ++i) {
//             CT_CHECK(arr[i] == i32(i + 1));
//             CT_CHECK(arr.at(i) == i32(i + 1));
//         }

//         arr.clear();

//         arr.append(1);
//         for (addr_size i = 0; i < 1; ++i) {
//             CT_CHECK(arr[i] == i32(i + 1));
//             CT_CHECK(arr.at(i) == i32(i + 1));
//         }

//         arr.append(2);
//         for (addr_size i = 0; i < 2; ++i) {
//             CT_CHECK(arr[i] == i32(i + 1));
//             CT_CHECK(arr.at(i) == i32(i + 1));
//         }

//         arr.append(3);
//         CT_CHECK(arr.len() == 3);
//         CT_CHECK(arr.cap() >= arr.len());
//         for (addr_size i = 0; i < 3; ++i) {
//             CT_CHECK(arr[i] == i32(i + 1));
//             CT_CHECK(arr.at(i) == i32(i + 1));
//         }

//         arr.clear();

//         // Append many trivial values.
//         i32 many[5] = { 1, 2, 3, 4, 5 };
//         arr.append(many, 5);
//         CT_CHECK(arr.len() == 5);
//         CT_CHECK(arr.cap() >= arr.len());
//         for (addr_size i = 0; i < 5; ++i) {
//             CT_CHECK(arr[i] == i32(i + 1));
//             CT_CHECK(arr.at(i) == i32(i + 1));
//         }
//     }

//     {
//         defer { CT::resetAll(); };
//         CT lv;
//         CT::resetCtors(); // Don't count the default ctors of the above code

//         {
//             core::Arr<CT, TAllocator> arr;
//             arr.append(CT{});
//             arr.append(lv);
//             CT_CHECK(CT::copyCtorCalled() == 1);
//             CT_CHECK(CT::moveCtorCalled() == 1);
//             for (addr_size i = 0; i < arr.len(); ++i) {
//                 CT_CHECK(arr[i].a == 7, "Append did not call the default constructors.");
//             }
//         }
//         CT_CHECK(CT::dtorsCalled() == 3);
//         CT::resetAll();

//         // Testing a combination of append and ensureCap.
//         {
//             core::Arr<CT, TAllocator> arr;
//             arr.ensureCap(1);
//             CT_CHECK(arr.len() == 0);
//             CT_CHECK(arr.cap() == 1);
//             CT_CHECK(CT::noCtorsCalled());

//             arr.append(CT{}); // calls ctor and dtor
//             arr.append(lv);
//             for (addr_size i = 0; i < arr.len(); ++i) {
//                 CT_CHECK(arr[i].a == 7, "Append did not call the default constructors.");
//             }
//             CT_CHECK(arr.len() == 2);
//             CT_CHECK(arr.cap() == 2);
//             arr.free(); // This should call the destructors.
//             CT_CHECK(arr.len() == 0);
//             CT_CHECK(arr.cap() == 0);
//             CT_CHECK(CT::dtorsCalled() == 3);
//         }
//         CT_CHECK(CT::dtorsCalled() == 3);
//         CT::resetAll();

//         // Test appending multiple values
//         CT staticArr[5];
//         CT::resetCtors(); // Don't count the default ctors of the above code
//         {
//             core::Arr<CT, TAllocator> arr;
//             arr.append(staticArr, 5);
//             CT_CHECK(arr.len() == 5);
//             CT_CHECK(arr.cap() >= arr.len());
//             CT_CHECK(CT::copyCtorCalled() == 5);
//             CT_CHECK(CT::moveCtorCalled() == 0);
//             CT_CHECK(CT::defaultCtorCalled() == 0);
//             for (addr_size i = 0; i < arr.len(); ++i) {
//                 CT_CHECK(arr[i].a == 7, "Append multiple did not call the default constructors.");
//             }
//         }
//         CT_CHECK(CT::dtorsCalled() == 5);
//         CT::resetAll();
//     }

//     {
//         // Appending arrays that are bigger than double current capacity.
//         core::Arr<i32, TAllocator> arr(0, 2);
//         core::Arr<i32, TAllocator> arr2;
//         arr2.append(1).append(2).append(3).append(4).append(5).append(6).append(7).append(8).append(9).append(10);

//         arr.append(arr2);
//         CT_CHECK(arr.len() == 10);
//         CT_CHECK(arr.cap() >= arr.len());
//         for (addr_size i = 0; i < 10; ++i) {
//             CT_CHECK(arr[i] == i32(i + 1));
//             CT_CHECK(arr.at(i) == i32(i + 1));
//         }
//     }

//     return 0;
// }

// template <typename TAllocator>
// i32 arrayOfArraysArrTest() {
//     {
//         core::Arr<i32, TAllocator> arr;
//         core::Arr<i32, TAllocator> arr2;
//         core::Arr<i32, TAllocator> arr3;
//         core::Arr<core::Arr<i32, TAllocator>, TAllocator> multi;

//         arr.append(1);
//         arr.append(2);
//         arr.append(3);

//         arr2.append(4);
//         arr2.append(5);
//         arr2.append(6);

//         arr3.append(7);
//         arr3.append(8);
//         arr3.append(9);

//         multi.append(arr.copy());
//         multi.append(std::move(arr2));
//         multi.append(std::move(arr3));

//         // arr 1 should be copied
//         CT_CHECK(arr.len() == 3);
//         CT_CHECK(arr[0] == 1);
//         CT_CHECK(arr[1] == 2);
//         CT_CHECK(arr[2] == 3);

//         // arr 2 and 3 should have been moved
//         CT_CHECK(arr2.len() == 0);
//         CT_CHECK(arr2.data() == nullptr);
//         CT_CHECK(arr3.len() == 0);
//         CT_CHECK(arr3.data() == nullptr);

//         CT_CHECK(multi.len() == 3);
//         CT_CHECK(multi[0].len() == 3);
//         CT_CHECK(multi[1].len() == 3);
//         CT_CHECK(multi[2].len() == 3);
//         CT_CHECK(multi[0][0] == 1);
//         CT_CHECK(multi[0][1] == 2);
//         CT_CHECK(multi[0][2] == 3);
//         CT_CHECK(multi[1][0] == 4);
//         CT_CHECK(multi[1][1] == 5);
//         CT_CHECK(multi[1][2] == 6);
//         CT_CHECK(multi[2][0] == 7);
//         CT_CHECK(multi[2][1] == 8);
//         CT_CHECK(multi[2][2] == 9);
//     }

//     return 0;
// }

// template <typename TAllocator>
// i32 clearArrayShouldCallDtorsTest() {
//     using CT = core::testing::CT;

//     defer { CT::resetAll(); };
//     constexpr i32 testCount = 10;
//     auto arr = core::Arr<CT, TAllocator>(testCount);
//     arr.clear();
//     CT_CHECK(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
//     CT_CHECK(arr.cap() == testCount, "Clear should not change the capacity of the array.");

//     CT::resetDtors();

//     arr.clear();
//     arr = core::Arr<CT, TAllocator>(testCount);
//     arr.clear();
//     CT_CHECK(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
//     CT_CHECK(arr.cap() == testCount, "Clear should not change the capacity of the array.");

//     return 0;
// }

// template <typename TAllocator>
// i32 removeFromArrayTest() {
//     using CT = core::testing::CT;

//     {
//         core::Arr<i32, TAllocator> arr;

//         {
//             arr.append(1).remove(0);
//             CT_CHECK(arr.len() == 0);
//             arr.clear();
//         }
//         {
//             arr.append(1).append(2).remove(arr.len() - 1);
//             CT_CHECK(arr.len() == 1);
//             CT_CHECK(arr[0] == 1);
//             arr.clear();
//         }
//         {
//             arr.append(1).append(2).remove(0);
//             CT_CHECK(arr.len() == 1);
//             CT_CHECK(arr[0] == 2);
//             arr.clear();
//         }
//     }

//     {
//         core::Arr<CT, TAllocator> arr;

//         defer { CT::resetAll(); };

//         CT a, b;

//         {
//             CT::resetDtors();
//             arr.append(a).remove(0);
//             CT_CHECK(CT::dtorsCalled() == 1);
//             CT_CHECK(arr.len() == 0);
//             arr.clear();
//         }
//         {
//             CT::resetDtors();
//             arr.append(a).append(b).remove(arr.len() - 1);
//             CT_CHECK(CT::dtorsCalled() == 1);
//             CT_CHECK(arr.len() == 1);
//             arr.clear();
//         }
//         {
//             CT::resetDtors();
//             arr.append(a).append(b).remove(0);
//             CT_CHECK(CT::dtorsCalled() == 1);
//             CT_CHECK(arr.len() == 1);
//             arr.clear();
//         }
//     }

//     return 0;
// }

// template <typename TAllocator>
// i32 resetArrayTest() {
//     auto unmanaged = reinterpret_cast<u8*>(TAllocator::alloc(256));
//     core::memset(unmanaged, 5, 256);

//     core::Arr<u8, TAllocator> arr;
//     arr.reset(unmanaged, 256); // arr becomes the owner and thus must free the memory when it goes out of scope.

//     CT_CHECK(arr.len() == 256);
//     CT_CHECK(arr.cap() == 256);
//     CT_CHECK(arr.data() == unmanaged);

//     for (addr_size i = 0; i < arr.len(); ++i) {
//         CT_CHECK(arr[i] == 5);
//     }

//     return 0;
// }

i32 runArrTestsSuite() {
    using namespace core::testing;

    auto runTests = [] (TestInfo& tInfo, const char* description, i32& retCode) {
        tInfo.description = description;

        tInfo.name = FN_NAME_TO_CPTR(initializeArrTest);
        if (runTest(tInfo, initializeArrTest) != 0) { retCode = -1; }

        tInfo.name = FN_NAME_TO_CPTR(ensureArrCapacity);
        if (runTest(tInfo, ensureArrCapacity) != 0) { retCode = -1; }
    };

    i32 ret = 0;
    runForAllGlobalAllocatorVariants(runTests, ret);

    {
        constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE;
        char buf[BUFFER_SIZE];
        core::BumpAllocator localBumpAllocator(buf, BUFFER_SIZE);
        defer {
            // It's not strictly necessary to clear here, because the buffer is on the stack, but it's good practice.
            localBumpAllocator.clear();
        };
        core::AllocatorContext localBumpAllocatorCtx = core::createAllocatorCtx(&localBumpAllocator);
        core::setActiveAllocatorForThread(&localBumpAllocatorCtx);
        defer { core::setActiveAllocatorForThread(nullptr); };

        TestInfo tInfo = {};
        runTests(tInfo, "STACK BASED BUMP Allocator", ret);
    }

    {
        constexpr u32 BUFFER_SIZE = 256; // intentially small to test overflow.
        core::StdArenaAllocator customArenaAllocator (BUFFER_SIZE);
        defer { customArenaAllocator.clear(); }; // need to free the memory manually.
        core::AllocatorContext customArenaAllocatorCtx = core::createAllocatorCtx(&customArenaAllocator);
        core::setActiveAllocatorForThread(&customArenaAllocatorCtx);
        defer { core::setActiveAllocatorForThread(nullptr); };

        TestInfo tInfo = {};
        runTests(tInfo, "CUSTOM ARENA Allocator", ret);
    }

    return ret;
}
