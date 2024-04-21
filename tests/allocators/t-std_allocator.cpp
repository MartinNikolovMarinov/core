#include "../t-index.h"

i32 stdAllocatorBasicValidityTest() {
    core::StdAllocator allocator;

    {
        u8* data = reinterpret_cast<u8*>(allocator.alloc(4, sizeof(u8)));
        CT_CHECK(data != nullptr);
        allocator.free(data, 4, sizeof(u8));
        allocator.inUseMemory(); // does nothing, but should not crash
        allocator.totalMemoryAllocated(); // does nothing, but should not crash
    }

    {
        struct TestStruct {
            i32 a;
            f32 b;

            TestStruct() : a(0), b(0.0f) {}
            TestStruct(i32 _a, f32 _b) : a(_a), b(_b) {}
        };

        {
            TestStruct* ts = reinterpret_cast<TestStruct*>(allocator.alloc(1, sizeof(TestStruct)));
            CT_CHECK(ts != nullptr);
            allocator.free(ts, 1, sizeof(TestStruct));
        }
        {
            TestStruct* ts = reinterpret_cast<TestStruct*>(allocator.calloc(1, sizeof(TestStruct)));
            CT_CHECK(ts != nullptr);
            CT_CHECK(ts->a == 0);
            CT_CHECK(ts->b == 0.0f);
            allocator.free(ts, 1, sizeof(TestStruct));
        }

        allocator.inUseMemory(); // does nothing, but should not crash
        allocator.totalMemoryAllocated(); // does nothing, but should not crash
    }

    return 0;
}

i32 onOOMStdAllocatorTest() {
    static i32 testOOMCount = 0;
    void* data;
    core::StdAllocator allocator;

    CT_CHECK(allocator.oomHandler == core::defaultOOMHandler);

    allocator.oomHandler = []() { testOOMCount++; };

    data = allocator.alloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr);
    CT_CHECK(testOOMCount == 1);

    data = allocator.calloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr);
    CT_CHECK(testOOMCount == 2);

    allocator.oomHandler = nullptr;
    data = allocator.alloc(0x7ffffffffffffffd, sizeof(u32));
    CT_CHECK(data == nullptr, "Setting the oomHandler to null is not properly handled.");

    return 0;
}

i32 runStdAllocatorTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;

    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(stdAllocatorBasicValidityTest);
    if (runTest(tInfo, stdAllocatorBasicValidityTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(onOOMStdAllocatorTest);
    if (runTest(tInfo, onOOMStdAllocatorTest) != 0) { ret = -1; }

    return ret;
}
