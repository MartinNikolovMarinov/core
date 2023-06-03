template<typename TAllocator>
void initialize_arr() {
    core::arr<i32, TAllocator> arr;
    Assert(arr.len() == 0);
    Assert(arr.cap() == 0);
    Assert(arr.data() == nullptr);
    Assert(arr.empty());

    core::arr<i32, TAllocator> arr2(10);
    Assert(arr2.len() == 10);
    Assert(arr2.cap() == 10);
    Assert(arr2.data() != nullptr);
    Assert(!arr2.empty());

    core::arr<i32, TAllocator> arr3(10, 20);
    Assert(arr3.len() == 10);
    Assert(arr3.cap() == 20);
    Assert(arr3.data() != nullptr);
    Assert(!arr3.empty());
}

template<typename TAllocator>
void move_and_copy_arr() {
    core::arr<i32, TAllocator> arr(10);
    arr.fill(1);
    core::arr<i32, TAllocator> arrCpy;
    core::arr<i32, TAllocator> arrMoved(10);

    arrCpy = arr.copy();
    Assert(arrCpy.len() == arr.len());
    Assert(arrCpy.cap() == arr.cap());
    Assert(arrCpy.data() != arr.data());
    Assert(!arrCpy.empty());
    for (i32 i = 0; i < arrCpy.len(); ++i) {
        Assert(arrCpy[i] == arr[i]);
    }

    arrMoved = core::move(arr);
    Assert(arr.len() == 0);
    Assert(arr.cap() == 0);
    Assert(arr.data() == nullptr);
    Assert(arr.empty());
    Assert(arrMoved.len() == arrCpy.len());
    Assert(arrMoved.cap() == arrCpy.cap());
    Assert(!arrMoved.empty());
    for (i32 i = 0; i < arrMoved.len(); ++i) {
        Assert(arrMoved[i] == arrCpy[i]);
    }
}

template<typename TAllocator>
void resize_arr() {
    core::arr<i32, TAllocator> arr;
    Assert(arr.len() == 0);
    Assert(arr.cap() == 0);
    Assert(arr.data() == nullptr);
    Assert(arr.empty());

    arr.resize(10);
    Assert(arr.len() == 0);
    Assert(arr.cap() == 10);
    Assert(arr.data() != nullptr);
    Assert(arr.empty());

    arr.resize(0);
    Assert(arr.len() == 0);
    Assert(arr.cap() == 0);
    Assert(arr.data() != nullptr);
    Assert(arr.empty());
}

template<typename TAllocator>
void fill_arr() {
    {
        core::arr<i32, TAllocator> arr;
        arr.fill(0); // should not crash
        Assert(arr.len() == 0);
    }

    {
        core::arr<i32, TAllocator> arr(10);

        arr.fill(0);
        for (i32 i = 0; i < arr.len(); ++i) {
            Assert(arr[i] == 0);
        }

        arr.fill(1);
        for (i32 i = 0; i < arr.len(); ++i) {
            Assert(arr[i] == 1);
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
        for (i32 i = 0; i < arr2.len(); ++i) {
            Assert(arr2[i]->a == 1);
            Assert(arr2[i]->b == 2.0);
        }

        arr2.fill(nullptr);
        for (i32 i = 0; i < arr2.len(); ++i) {
            Assert(arr2[i] == nullptr);
        }
    }

    {
        static constexpr i32 testCount = 5;
        static i32 destructorsCalled = 0;
        // fill with non-trivial struct
        struct TestStruct {
            i32 a;
            TestStruct() : a(7) {}
            ~TestStruct() { destructorsCalled++; }
        };
        auto v = TestStruct{};

        {
            core::arr<TestStruct, TAllocator> arr(testCount);
            arr.fill(v);
            for (i32 i = 0; i < arr.len(); ++i) {
                Assert(arr[i].a == 7);
                arr[i].a = 8;
            }
        }
        Assert(destructorsCalled == testCount, "Destructors where not called after the array went out of scope.");
        Assert(v.a == 7, "The value passed to fill was modified.");

        destructorsCalled = 0;

        {
            core::arr<TestStruct, TAllocator> arr(testCount);
            core::arr<TestStruct, TAllocator> arr2;
            arr.fill(v);
            arr = core::move(arr2);
            Assert(destructorsCalled == testCount, "Destructors were not called after a move assignment.");
        }

        destructorsCalled = 0;
    }
}

template<typename TAllocator>
void append_arr() {
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

    arr.resize(2);

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
}

template<typename TAllocator>
void array_of_arrays_arr() {
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

        multi.append(arr);
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
}
