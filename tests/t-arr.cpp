#include "index.h"

template <typename TAllocator>
i32 initializeArr() {
    {
        core::Arr<i32, TAllocator> Arr;
        Assert(Arr.len() == 0);
        Assert(Arr.cap() == 0);
        Assert(Arr.data() == nullptr);
        Assert(Arr.empty());
    }

    {
        core::Arr<i32, TAllocator> Arr(10);
        Assert(Arr.len() == 10);
        Assert(Arr.cap() == 10);
        Assert(!Arr.empty());

        for (addr_size i = 0; i < Arr.len(); ++i) {
            Assert(Arr[i] == 0);
        }
    }

    {
        core::Arr<i32, TAllocator> Arr(10, 20);
        Assert(Arr.len() == 10);
        Assert(Arr.cap() == 20);
        Assert(!Arr.empty());

        for (addr_size i = 0; i < Arr.len(); ++i) {
            Assert(Arr[i] == 0);
        }
    }

    {
        defer { SVCT::nextId = 0; };

        Assert(SVCT::nextId == 0);
        core::Arr<SVCT, TAllocator> Arr(10);
        Assert(Arr.len() == 10);
        Assert(Arr.cap() == 10);
        Assert(!Arr.empty());

        for (addr_size i = 0; i < Arr.len(); ++i) {
            Assert(Arr[i].a == i32(i));
        }
    }

    {
        defer { CT::resetAll(); };
        constexpr i32 testCount = 10;
        {
            core::Arr<CT, TAllocator> Arr(testCount);
            for (addr_size i = 0; i < Arr.len(); ++i) {
                Assert(Arr[i].a == 7, "Initializer did not call constructors!");
            }
            Assert(CT::totalCtorsCalled() == testCount, "Initializer did not call the exact number of constructors!");
            Assert(CT::defaultCtorCalled() == testCount, "Initializer did not call the exact number of copy constructors!");
            CT::resetCtors();
            {
                auto arrCpy = Arr.copy();
                Assert(arrCpy.data() != Arr.data());
                for (addr_size i = 0; i < arrCpy.len(); ++i) {
                    Assert(arrCpy[i].a == 7, "Copy constructor did not call constructors!");
                }
                Assert(CT::totalCtorsCalled() == testCount, "Copy constructor did not call the exact number of constructors!");
                Assert(CT::copyCtorCalled() == testCount, "Copy constructor did not call the exact number of copy constructors!");
                CT::resetCtors();
            }
            Assert(CT::dtorsCalled() == testCount, "Copy constructor did not call destructors!");
            CT::resetDtors();
        }
        Assert(CT::dtorsCalled() == testCount, "Copy constructor did not call destructors!");
        CT::resetDtors();
    }

    return 0;
}

template <typename TAllocator>
i32 moveAndCopyArr() {
    core::Arr<i32, TAllocator> Arr(10);
    Arr.fill(1);
    core::Arr<i32, TAllocator> arrCpy;
    core::Arr<i32, TAllocator> arrMoved(10);

    arrCpy = Arr.copy();
    Assert(arrCpy.len() == Arr.len());
    Assert(arrCpy.cap() == Arr.cap());
    Assert(arrCpy.data() != Arr.data());
    Assert(!arrCpy.empty());
    for (addr_size i = 0; i < arrCpy.len(); ++i) {
        Assert(arrCpy[i] == Arr[i]);
    }

    auto arrCpy2 = Arr.copy();
    Assert(arrCpy2.len() == Arr.len());
    Assert(arrCpy2.cap() == Arr.cap());
    Assert(arrCpy2.data() != Arr.data());
    Assert(!arrCpy2.empty());
    for (addr_size i = 0; i < arrCpy2.len(); ++i) {
        Assert(arrCpy2[i] == Arr[i]);
    }

    arrMoved = core::move(Arr);
    Assert(Arr.len() == 0);
    Assert(Arr.cap() == 0);
    Assert(Arr.data() == nullptr);
    Assert(Arr.empty());
    Assert(arrMoved.len() == arrCpy.len());
    Assert(arrMoved.cap() == arrCpy.cap());
    Assert(!arrMoved.empty());
    for (addr_size i = 0; i < arrMoved.len(); ++i) {
        Assert(arrMoved[i] == arrCpy[i]);
    }

    return 0;
}

template <typename TAllocator>
i32 resizeArr() {
    {
        core::Arr<i32, TAllocator> Arr;
        Assert(Arr.len() == 0);
        Assert(Arr.cap() == 0);
        Assert(Arr.data() == nullptr);
        Assert(Arr.empty());

        Arr.reserve(10);
        Assert(Arr.len() == 0);
        Assert(Arr.cap() == 10);
        Assert(Arr.data() != nullptr);
        Assert(Arr.empty());

        Arr.reserve(0);
        Assert(Arr.len() == 0);
        Assert(Arr.cap() == 0);
        Assert(Arr.data() != nullptr);
        Assert(Arr.empty());
    }

    return 0;
}

template <typename TAllocator>
i32 fillArr() {
    {
        core::Arr<i32, TAllocator> Arr;
        Arr.fill(0); // should not crash
        Assert(Arr.len() == 0);
    }

    {
        constexpr i32 N = 10;
        core::Arr<i32, TAllocator> Arr(N);

        Arr.fill(0);
        for (addr_size i = 0; i < N; ++i) {
            Assert(Arr[i] == 0);
        }

        Arr.fill(1);
        for (addr_size i = 0; i < N; ++i) {
            Assert(Arr[i] == 1);
        }
    }

    {
        constexpr i32 N = 1;
        core::Arr<i32, TAllocator> Arr(N, 5);

        Arr.fill(0);
        for (addr_size i = 0; i < N; ++i) {
            Assert(Arr[i] == 0);
            Assert(i < N, "Fill should use the length of the array, not the capacity.");
        }

        Arr.fill(1);
        for (addr_size i = 0; i < N; ++i) {
            Assert(Arr[i] == 1);
            Assert(i < N, "Fill should use the length of the array, not the capacity.");
        }
    }

    {
        // fill with struct
        struct TestStruct {
            i32 a;
            f64 b;
        };
        core::Arr<TestStruct*, TAllocator> arr2(10);
        TestStruct t = { 1, 2.0 };

        arr2.fill(&t);
        for (addr_size i = 0; i < arr2.len(); ++i) {
            Assert(arr2[i]->a == 1);
            Assert(arr2[i]->b == 2.0);
        }

        arr2.fill(nullptr);
        for (addr_size i = 0; i < arr2.len(); ++i) {
            Assert(arr2[i] == nullptr);
        }
    }

    {
        defer { CT::resetAll(); };
        constexpr i32 testCount = 10;
        CT v;

        {
            core::Arr<CT, TAllocator> Arr(testCount);
            Arr.fill(v);
            for (addr_size i = 0; i < Arr.len(); ++i) {
                Assert(Arr[i].a == 7, "Fill did not call the default constructors.");
                Arr[i].a = 8;
            }
        }
        Assert(CT::dtorsCalled() == testCount, "Destructors where not called after the array went out of scope.");
        Assert(v.a == 7, "The value passed to the fill function was modified.");
        CT::resetDtors();

        {
            core::Arr<CT, TAllocator> Arr(testCount);
            core::Arr<CT, TAllocator> arr2;
            Arr.fill(v);
            Arr = core::move(arr2);
            Assert(CT::dtorsCalled() == testCount, "Destructors were not called after a move assignment.");
        }
        CT::resetDtors();
    }

    return 0;
}

template <typename TAllocator>
i32 appendArr() {
    {
        core::Arr<i32, TAllocator> Arr;

        Arr.append(1);
        for (addr_size i = 0; i < 1; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.append(2);
        for (addr_size i = 0; i < 2; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.clear();

        Arr.append(1);
        for (addr_size i = 0; i < 1; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.append(2);
        for (addr_size i = 0; i < 2; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.append(3);
        Assert(Arr.len() == 3);
        Assert(Arr.cap() >= Arr.len());
        for (addr_size i = 0; i < 3; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.reserve(2);

        Arr.append(3);
        Assert(Arr.len() == 3);
        Assert(Arr.cap() >= Arr.len());
        for (addr_size i = 0; i < 3; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.append(4);
        Assert(Arr.len() == 4);
        Assert(Arr.cap() >= Arr.len());
        for (addr_size i = 0; i < 4; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.append(5);
        Assert(Arr.len() == 5);
        Assert(Arr.cap() >= Arr.len());
        for (addr_size i = 0; i < 5; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }

        Arr.clear();

        // Append many trivial values.
        i32 many[5] = { 1, 2, 3, 4, 5 };
        Arr.append(many, 5);
        Assert(Arr.len() == 5);
        Assert(Arr.cap() >= Arr.len());
        for (addr_size i = 0; i < 5; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }
    }

    {
        defer { CT::resetAll(); };
        CT lv;
        CT::resetCtors(); // Don't count the default ctors of the above code

        {
            core::Arr<CT, TAllocator> Arr;
            Arr.append(CT{});
            Arr.append(lv);
            Assert(CT::copyCtorCalled() == 1);
            Assert(CT::moveCtorCalled() == 1);
            for (addr_size i = 0; i < Arr.len(); ++i) {
                Assert(Arr[i].a == 7, "Append did not call the default constructors.");
            }
        }
        Assert(CT::dtorsCalled() == 3);
        CT::resetAll();

        // Testing a combination of append and reserve.
        {
            core::Arr<CT, TAllocator> Arr;
            Arr.reserve(1);
            Assert(Arr.len() == 0);
            Assert(Arr.cap() == 1);
            Assert(CT::noCtorsCalled());

            Arr.append(CT{}); // calls ctor and dtor
            Arr.append(lv);
            for (addr_size i = 0; i < Arr.len(); ++i) {
                Assert(Arr[i].a == 7, "Append did not call the default constructors.");
            }
            Assert(Arr.len() == 2);
            Assert(Arr.cap() == 2);
            Arr.reserve(0); // This reserve should call the destructors.
            Assert(Arr.len() == 0);
            Assert(Arr.cap() == 0);
            Assert(CT::dtorsCalled() == 3);
        }
        Assert(CT::dtorsCalled() == 3);
        CT::resetAll();

        // Test appending multiple values
        CT staticArr[5];
        CT::resetCtors(); // Don't count the default ctors of the above code
        {
            core::Arr<CT, TAllocator> Arr;
            Arr.append(staticArr, 5);
            Assert(Arr.len() == 5);
            Assert(Arr.cap() >= Arr.len());
            Assert(CT::copyCtorCalled() == 5);
            Assert(CT::moveCtorCalled() == 0);
            Assert(CT::defaultCtorCalled() == 0);
            for (addr_size i = 0; i < Arr.len(); ++i) {
                Assert(Arr[i].a == 7, "Append multiple did not call the default constructors.");
            }
        }
        Assert(CT::dtorsCalled() == 5);
        CT::resetAll();
    }

    {
        // Appending arrays that are bigger than double current capacity.
        core::Arr<i32, TAllocator> Arr(0, 2);
        core::Arr<i32, TAllocator> arr2;
        arr2.append(1).append(2).append(3).append(4).append(5).append(6).append(7).append(8).append(9).append(10);

        Arr.append(arr2);
        Assert(Arr.len() == 10);
        Assert(Arr.cap() >= Arr.len());
        for (addr_size i = 0; i < 10; ++i) {
            Assert(Arr[i] == i32(i + 1));
            Assert(Arr.at(i) == i32(i + 1));
        }
    }

    return 0;
}

template <typename TAllocator>
i32 arrayOfArraysArr() {
    {
        core::Arr<i32, TAllocator> Arr;
        core::Arr<i32, TAllocator> arr2;
        core::Arr<i32, TAllocator> arr3;
        core::Arr<core::Arr<i32, TAllocator>, TAllocator> multi;

        Arr.append(1);
        Arr.append(2);
        Arr.append(3);

        arr2.append(4);
        arr2.append(5);
        arr2.append(6);

        arr3.append(7);
        arr3.append(8);
        arr3.append(9);

        multi.append(Arr.copy());
        multi.append(core::move(arr2));
        multi.append(core::move(arr3));

        // Arr 1 should be copied
        Assert(Arr.len() == 3);
        Assert(Arr[0] == 1);
        Assert(Arr[1] == 2);
        Assert(Arr[2] == 3);

        // Arr 2 and 3 should have been moved
        Assert(arr2.len() == 0);
        Assert(arr2.data() == nullptr);
        Assert(arr3.len() == 0);
        Assert(arr3.data() == nullptr);

        Assert(multi.len() == 3);
        Assert(multi[0].len() == 3);
        Assert(multi[1].len() == 3);
        Assert(multi[2].len() == 3);
        Assert(multi[0][0] == 1);
        Assert(multi[0][1] == 2);
        Assert(multi[0][2] == 3);
        Assert(multi[1][0] == 4);
        Assert(multi[1][1] == 5);
        Assert(multi[1][2] == 6);
        Assert(multi[2][0] == 7);
        Assert(multi[2][1] == 8);
        Assert(multi[2][2] == 9);
    }

    return 0;
}

template <typename TAllocator>
i32 clearArrayShouldCallDtorsTest() {
    defer { CT::resetAll(); };
    constexpr i32 testCount = 10;
    auto Arr = core::Arr<CT, TAllocator>(testCount);
    Arr.clear();
    Assert(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
    Assert(Arr.cap() == testCount, "Clear should not change the capacity of the array.");

    CT::resetDtors();

    Arr.clear();
    Arr = core::Arr<CT, TAllocator>(testCount);
    Arr.clear();
    Assert(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
    Assert(Arr.cap() == testCount, "Clear should not change the capacity of the array.");

    return 0;
}

i32 runArrTestsSuite() {

    constexpr addr_size BUFF_SIZE = core::KILOBYTE;
    char buf[BUFF_SIZE];

    core::StdAllocator::init(nullptr);
    core::StdStatsAllocator::init(nullptr);
    core::BumpAllocator::init(nullptr, buf, BUFF_SIZE);

    auto checkLeaks = []() {
        Assert(core::StdAllocator::usedMem() == 0);
        Assert(core::StdStatsAllocator::usedMem() == 0, "Memory leak detected!");
        Assert(core::BumpAllocator::usedMem() == 0);
    };

    {
        RunTest(initializeArr<core::StdAllocator>);
        RunTest(initializeArr<core::StdStatsAllocator>);
        RunTest(initializeArr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(moveAndCopyArr<core::StdAllocator>);
        RunTest(moveAndCopyArr<core::StdStatsAllocator>);
        RunTest(moveAndCopyArr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(resizeArr<core::StdAllocator>);
        RunTest(resizeArr<core::StdStatsAllocator>);
        RunTest(resizeArr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(fillArr<core::StdAllocator>);
        RunTest(fillArr<core::StdStatsAllocator>);
        RunTest(fillArr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(appendArr<core::StdAllocator>);
        RunTest(appendArr<core::StdStatsAllocator>);
        RunTest(appendArr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(arrayOfArraysArr<core::StdAllocator>);
        RunTest(arrayOfArraysArr<core::StdStatsAllocator>);
        RunTest(arrayOfArraysArr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(clearArrayShouldCallDtorsTest<core::StdAllocator>);
        RunTest(clearArrayShouldCallDtorsTest<core::StdStatsAllocator>);
        RunTest(clearArrayShouldCallDtorsTest<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }

    return 0;
}
