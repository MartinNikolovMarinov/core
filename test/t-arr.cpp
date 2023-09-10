struct __test_with_default_ctor {
    static i32 itter;
    i32 a = itter++; // even this nonsense should work.
};
i32 __test_with_default_ctor::itter = 0;

template <typename TAllocator>
i32 initialize_arr() {
    {
        core::arr<i32, TAllocator> arr;
        Assert(arr.len() == 0);
        Assert(arr.cap() == 0);
        Assert(arr.data() == nullptr);
        Assert(arr.empty());
    }

    {
        core::arr<i32, TAllocator> arr(10);
        Assert(arr.len() == 10);
        Assert(arr.cap() == 10);
        Assert(!arr.empty());

        for (i32 i = 0; i < i32(arr.len()); ++i) {
            Assert(arr[i] == 0);
        }
    }

    {
        core::arr<i32, TAllocator> arr(10, 20);
        Assert(arr.len() == 10);
        Assert(arr.cap() == 20);
        Assert(!arr.empty());

        for (i32 i = 0; i < i32(arr.len()); ++i) {
            Assert(arr[i] == 0);
        }
    }

    {
        __test_with_default_ctor::itter = 0;
        core::arr<__test_with_default_ctor, TAllocator> arr(10);
        Assert(arr.len() == 10);
        Assert(arr.cap() == 10);
        Assert(!arr.empty());

        for (i32 i = 0; i < i32(arr.len()); ++i) {
            Assert(arr[i].a == i);
        }
    }

    {
        defer { CT::resetAll(); };
        constexpr i32 testCount = 10;
        {
            core::arr<CT, TAllocator> arr(testCount);
            for (i32 i = 0; i < i32(arr.len()); ++i) {
                Assert(arr[i].a == 7, "Initializer did not call constructors!");
            }
            Assert(CT::totalCtorsCalled() == testCount, "Initializer did not call the exact number of constructors!");
            Assert(CT::defaultCtorCalled() == testCount, "Initializer did not call the exact number of copy constructors!");
            CT::resetCtors();
            {
                auto arrCpy = arr.copy();
                Assert(arrCpy.data() != arr.data());
                for (i32 i = 0; i < i32(arrCpy.len()); ++i) {
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
i32 move_and_copy_arr() {
    core::arr<i32, TAllocator> arr(10);
    arr.fill(1);
    core::arr<i32, TAllocator> arrCpy;
    core::arr<i32, TAllocator> arrMoved(10);

    arrCpy = arr.copy();
    Assert(arrCpy.len() == arr.len());
    Assert(arrCpy.cap() == arr.cap());
    Assert(arrCpy.data() != arr.data());
    Assert(!arrCpy.empty());
    for (i32 i = 0; i < i32(arrCpy.len()); ++i) {
        Assert(arrCpy[i] == arr[i]);
    }

    auto arrCpy2 = arr.copy();
    Assert(arrCpy2.len() == arr.len());
    Assert(arrCpy2.cap() == arr.cap());
    Assert(arrCpy2.data() != arr.data());
    Assert(!arrCpy2.empty());
    for (i32 i = 0; i < i32(arrCpy2.len()); ++i) {
        Assert(arrCpy2[i] == arr[i]);
    }

    arrMoved = core::move(arr);
    Assert(arr.len() == 0);
    Assert(arr.cap() == 0);
    Assert(arr.data() == nullptr);
    Assert(arr.empty());
    Assert(arrMoved.len() == arrCpy.len());
    Assert(arrMoved.cap() == arrCpy.cap());
    Assert(!arrMoved.empty());
    for (i32 i = 0; i < i32(arrMoved.len()); ++i) {
        Assert(arrMoved[i] == arrCpy[i]);
    }

    return 0;
}

template <typename TAllocator>
i32 resize_arr() {
    {
        core::arr<i32, TAllocator> arr;
        Assert(arr.len() == 0);
        Assert(arr.cap() == 0);
        Assert(arr.data() == nullptr);
        Assert(arr.empty());

        arr.reserve(10);
        Assert(arr.len() == 0);
        Assert(arr.cap() == 10);
        Assert(arr.data() != nullptr);
        Assert(arr.empty());

        arr.reserve(0);
        Assert(arr.len() == 0);
        Assert(arr.cap() == 0);
        Assert(arr.data() != nullptr);
        Assert(arr.empty());
    }

    return 0;
}

template <typename TAllocator>
i32 fill_arr() {
    {
        core::arr<i32, TAllocator> arr;
        arr.fill(0); // should not crash
        Assert(arr.len() == 0);
    }

    {
        constexpr i32 N = 10;
        core::arr<i32, TAllocator> arr(N);

        arr.fill(0);
        for (i32 i = 0; i < N; ++i) {
            Assert(arr[i] == 0);
        }

        arr.fill(1);
        for (i32 i = 0; i < N; ++i) {
            Assert(arr[i] == 1);
        }
    }

    {
        constexpr i32 N = 1;
        core::arr<i32, TAllocator> arr(N, 5);

        arr.fill(0);
        for (i32 i = 0; i < N; ++i) {
            Assert(arr[i] == 0);
            Assert(i < N, "Fill should use the length of the array, not the capacity.");
        }

        arr.fill(1);
        for (i32 i = 0; i < N; ++i) {
            Assert(arr[i] == 1);
            Assert(i < N, "Fill should use the length of the array, not the capacity.");
        }
    }

    {
        // fill with struct
        struct TestStruct {
            i32 a;
            f64 b;
        };
        core::arr<TestStruct*, TAllocator> arr2(10);
        TestStruct t = { 1, 2.0 };

        arr2.fill(&t);
        for (i32 i = 0; i < i32(arr2.len()); ++i) {
            Assert(arr2[i]->a == 1);
            Assert(arr2[i]->b == 2.0);
        }

        arr2.fill(nullptr);
        for (i32 i = 0; i < i32(arr2.len()); ++i) {
            Assert(arr2[i] == nullptr);
        }
    }

    {
        defer { CT::resetAll(); };
        constexpr i32 testCount = 10;
        CT v;

        {
            core::arr<CT, TAllocator> arr(testCount);
            arr.fill(v);
            for (i32 i = 0; i < i32(arr.len()); ++i) {
                Assert(arr[i].a == 7, "Fill did not call the default constructors.");
                arr[i].a = 8;
            }
        }
        Assert(CT::dtorsCalled() == testCount, "Destructors where not called after the array went out of scope.");
        Assert(v.a == 7, "The value passed to the fill function was modified.");
        CT::resetDtors();

        {
            core::arr<CT, TAllocator> arr(testCount);
            core::arr<CT, TAllocator> arr2;
            arr.fill(v);
            arr = core::move(arr2);
            Assert(CT::dtorsCalled() == testCount, "Destructors were not called after a move assignment.");
        }
        CT::resetDtors();
    }

    return 0;
}

template <typename TAllocator>
i32 append_arr() {
    {
        core::arr<i32, TAllocator> arr;

        arr.append(1);
        for (i32 i = 0; i < 1; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.append(2);
        for (i32 i = 0; i < 2; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.clear();

        arr.append(1);
        for (i32 i = 0; i < 1; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.append(2);
        for (i32 i = 0; i < 2; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.append(3);
        Assert(arr.len() == 3);
        Assert(arr.cap() >= arr.len());
        for (i32 i = 0; i < 3; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.reserve(2);

        arr.append(3);
        Assert(arr.len() == 3);
        Assert(arr.cap() >= arr.len());
        for (i32 i = 0; i < 3; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.append(4);
        Assert(arr.len() == 4);
        Assert(arr.cap() >= arr.len());
        for (i32 i = 0; i < 4; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.append(5);
        Assert(arr.len() == 5);
        Assert(arr.cap() >= arr.len());
        for (i32 i = 0; i < 5; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }

        arr.clear();

        // Append many trivial values.
        i32 many[5] = { 1, 2, 3, 4, 5 };
        arr.append(many, 5);
        Assert(arr.len() == 5);
        Assert(arr.cap() >= arr.len());
        for (i32 i = 0; i < 5; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }
    }

    {
        defer { CT::resetAll(); };
        CT lv;
        CT::resetCtors(); // Don't count the default ctors of the above code

        {
            core::arr<CT, TAllocator> arr;
            arr.append(CT{});
            arr.append(lv);
            Assert(CT::copyCtorCalled() == 1);
            Assert(CT::moveCtorCalled() == 1);
            for (i32 i = 0; i < i32(arr.len()); ++i) {
                Assert(arr[i].a == 7, "Append did not call the default constructors.");
            }
        }
        Assert(CT::dtorsCalled() == 3);
        CT::resetAll();

        // Testing a combination of append and reserve.
        {
            core::arr<CT, TAllocator> arr;
            arr.reserve(1);
            Assert(arr.len() == 0);
            Assert(arr.cap() == 1);
            Assert(CT::noCtorsCalled());

            arr.append(CT{}); // calls ctor and dtor
            arr.append(lv);
            for (i32 i = 0; i < i32(arr.len()); ++i) {
                Assert(arr[i].a == 7, "Append did not call the default constructors.");
            }
            Assert(arr.len() == 2);
            Assert(arr.cap() == 2);
            arr.reserve(0); // This reserve should call the destructors.
            Assert(arr.len() == 0);
            Assert(arr.cap() == 0);
            Assert(CT::dtorsCalled() == 3);
        }
        Assert(CT::dtorsCalled() == 3);
        CT::resetAll();

        // Test appending multiple values
        CT staticArr[5];
        CT::resetCtors(); // Don't count the default ctors of the above code
        {
            core::arr<CT, TAllocator> arr;
            arr.append(staticArr, 5);
            Assert(arr.len() == 5);
            Assert(arr.cap() >= arr.len());
            Assert(CT::copyCtorCalled() == 5);
            Assert(CT::moveCtorCalled() == 0);
            Assert(CT::defaultCtorCalled() == 0);
            for (i32 i = 0; i < i32(arr.len()); ++i) {
                Assert(arr[i].a == 7, "Append multiple did not call the default constructors.");
            }
        }
        Assert(CT::dtorsCalled() == 5);
        CT::resetAll();
    }

    {
        // Appending arrays that are bigger than double current capacity.
        core::arr<i32, TAllocator> arr(0, 2);
        core::arr<i32, TAllocator> arr2;
        arr2.append(1).append(2).append(3).append(4).append(5).append(6).append(7).append(8).append(9).append(10);

        arr.append(arr2);
        Assert(arr.len() == 10);
        Assert(arr.cap() >= arr.len());
        for (i32 i = 0; i < 10; ++i) {
            Assert(arr[i] == i + 1);
            Assert(arr.at(i) == i + 1);
        }
    }

    return 0;
}

template <typename TAllocator>
i32 array_of_arrays_arr() {
    {
        core::arr<i32, TAllocator> arr;
        core::arr<i32, TAllocator> arr2;
        core::arr<i32, TAllocator> arr3;
        core::arr<core::arr<i32, TAllocator>, TAllocator> multi;

        arr.append(1);
        arr.append(2);
        arr.append(3);

        arr2.append(4);
        arr2.append(5);
        arr2.append(6);

        arr3.append(7);
        arr3.append(8);
        arr3.append(9);

        multi.append(arr.copy());
        multi.append(core::move(arr2));
        multi.append(core::move(arr3));

        // arr 1 should be copied
        Assert(arr.len() == 3);
        Assert(arr[0] == 1);
        Assert(arr[1] == 2);
        Assert(arr[2] == 3);

        // arr 2 and 3 should have been moved
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
i32 clear_array_should_call_dtors_test() {
    defer { CT::resetAll(); };
    constexpr i32 testCount = 10;
    auto arr = core::arr<CT, TAllocator>(testCount);
    arr.clear();
    Assert(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
    Assert(arr.cap() == testCount, "Clear should not change the capacity of the array.");

    CT::resetDtors();

    arr.clear();
    arr = core::arr<CT, TAllocator>(testCount);
    arr.clear();
    Assert(CT::dtorsCalled() == testCount, "Clear should call dtors on all elements.");
    Assert(arr.cap() == testCount, "Clear should not change the capacity of the array.");

    return 0;
}

