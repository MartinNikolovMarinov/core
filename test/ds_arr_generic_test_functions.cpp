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
    core::arr<i32, TAllocator> arr(10);

    arr.fill(0);
    for (i32 i = 0; i < arr.len(); ++i) {
        Assert(arr[i] == 0);
    }

    arr.fill(1);
    for (i32 i = 0; i < arr.len(); ++i) {
        Assert(arr[i] == 1);
    }

    // fill with struct
    struct test_struct {
        i32 a;
        f64 b;
    };
    core::arr<test_struct*, TAllocator> arr2(10);
    test_struct t = { 1, 2.0 };

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
