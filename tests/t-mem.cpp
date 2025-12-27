#include "core_cstr.h"
#include "core_exec_ctx.h"
#include "core_macros.h"
#include "core_mem.h"
#include "core_types.h"
#include "t-index.h"
#include "testing/testing_framework.h"

namespace {

constexpr i32 alignTest() {
    struct TestCase {
        addr_size in;
        addr_size expected;
    };

    constexpr TestCase cases[] = {
        { 0, 0 },
        { 1, 8 },
        { 2, 8 },
        { 3, 8 },
        { 4, 8 },
        { 5, 8 },
        { 6, 8 },
        { 7, 8 },
        { 8, 8 },
        { 9, 16 },
        { 10, 16 },
        { 17, 24 },
        { 25, 32 }
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        auto got = core::align(c.in);
        CT_CHECK(got == c.expected, cErr);
        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

PRAGMA_WARNING_PUSH

DISABLE_GCC_AND_CLANG_WARNING(-Wconversion)
DISABLE_MSVC_WARNING(4244)

constexpr i32 memswapTest() {
    auto testCase = [](auto& a, auto& b, addr_size N) {
        for (addr_size i = 0; i < N; ++i) {
            a[i] = i;
            b[i] = i + N;
        }

        core::memswap(a, b, N);

        for (addr_size i = 0; i < N; ++i) {
            CT_CHECK(a[i] == i + N);
            CT_CHECK(b[i] == i);
        }

        return 0;
    };

    {
        // small
        u8 a[1]; u8 b[1];
        CT_CHECK(testCase(a, b, 1) == 0);
    }
    {
        // odd
        u8 a[5]; u8 b[5];
        CT_CHECK(testCase(a, b, 5) == 0);
    }
    {
        // even
        u8 a[6]; u8 b[6];
        CT_CHECK(testCase(a, b, 6) == 0);
    }
    {
        struct A {
            i32 a;
            u64 b;
            u8 c;
        };

        A a1 = { core::limitMax<i32>(), core::limitMax<u64>(), core::limitMax<u8>() };
        A a2 = { core::limitMin<i32>(), 0, 0 };
        core::memswap(&a1, &a2, 1);
        CT_CHECK(a1.a == core::limitMin<i32>());
        CT_CHECK(a1.b == 0);
        CT_CHECK(a1.c == 0);
        CT_CHECK(a2.a == core::limitMax<i32>());
        CT_CHECK(a2.b == core::limitMax<u64>());
        CT_CHECK(a2.c == core::limitMax<u8>());
    }
    {
        struct A {
            i32 a;
            u8 b[3];
            i32 c;
        };

        A a1 = { 1, {2, 3, 4}, 5 };
        A a2 = { 5, {6, 7, 8}, 9 };
        core::memswap(&a1, &a2, 1);
        CT_CHECK(a1.a == 5);
        CT_CHECK(a1.b[0] == 6);
        CT_CHECK(a1.b[1] == 7);
        CT_CHECK(a1.b[2] == 8);
        CT_CHECK(a1.c == 9);
        CT_CHECK(a2.a == 1);
        CT_CHECK(a2.b[0] == 2);
        CT_CHECK(a2.b[1] == 3);
        CT_CHECK(a2.b[2] == 4);
        CT_CHECK(a2.c == 5);
    }

    return 0;
}

constexpr i32 swapTest() {
    // Simple numeric types
    {
        i32 a = -7;
        i32 b = 13;
        core::swap(a, b);
        CT_CHECK(a == 13);
        CT_CHECK(b == -7);
    }
    {
        f64 a = 1.25;
        f64 b = -2.5;
        core::swap(a, b);
        CT_CHECK(a == -2.5);
        CT_CHECK(b == 1.25);
    }

    // Struct types
    {
        struct Pair {
            u16 x;
            u16 y;
        };

        Pair first{ 10, 11 };
        Pair second{ 20, 21 };
        core::swap(first, second);
        CT_CHECK(first.x == 20 && first.y == 21);
        CT_CHECK(second.x == 10 && second.y == 11);
    }

    {
        struct Complex {
            i64 id;
            char label[5];
            bool flags[3];
            u32 values[2];
        };

        Complex lhs{ 42, { 'a', 'b', 'c', 'd', '\0' }, { true, false, true }, { 7, 8 } };
        Complex rhs{ 7, { 'x', 'y', 'z', 'w', '\0' }, { false, true, false }, { 1, 2 } };

        core::swap(lhs, rhs);

        CT_CHECK(lhs.id == 7);
        CT_CHECK(lhs.label[0] == 'x' && lhs.label[1] == 'y' && lhs.label[2] == 'z' && lhs.label[3] == 'w' && lhs.label[4] == '\0');
        CT_CHECK(lhs.flags[0] == false && lhs.flags[1] == true && lhs.flags[2] == false);
        CT_CHECK(lhs.values[0] == 1 && lhs.values[1] == 2);

        CT_CHECK(rhs.id == 42);
        CT_CHECK(rhs.label[0] == 'a' && rhs.label[1] == 'b' && rhs.label[2] == 'c' && rhs.label[3] == 'd' && rhs.label[4] == '\0');
        CT_CHECK(rhs.flags[0] == true && rhs.flags[1] == false && rhs.flags[2] == true);
        CT_CHECK(rhs.values[0] == 7 && rhs.values[1] == 8);
    }

    return 0;
}

PRAGMA_WARNING_POP

constexpr i32 memcopyTest() {
    // Validate sequential calls to copy.
    {
        constexpr i32 N = 20;
        u16 sequence[N] = {};

        // Set the sequence from 1..N
        for (i32 i = 0; i < N; i++) {
            sequence[i] = u16(i);
        }

        for (i32 i = 0; i < N; i++) {
            u16 buf[N] = {};
            core::memcopy(buf, sequence, addr_size(i)); // copy the sequence into the buffer to i
            for (i32 j = 0; j < i; j++) {
                // CT_CHECK that the first i bytes are the same as the sequence
                CT_CHECK(buf[j] == j);
            }
            for (i32 j = i; j < N; j++) {
                // CT_CHECK that the rest are all zeroes
                CT_CHECK(buf[j] == 0);
            }
        }
    }

    // Validate pointer advance after copy.
    {
        constexpr addr_size N = 32;
        char buffer[N] = {};
        char* ptr = buffer;

        char first[] = "Hello,";
        const addr_size firstLen = core::cstrLen(first);
        ptr += core::memcopy(buffer, first, firstLen);

        for (addr_size i = 0; i < firstLen; i++) {
            CT_CHECK(buffer[i] == first[i]);
        }

        constexpr const char* second = " World!";
        constexpr addr_size secondLen = core::cstrLen(second);
        ptr += core::memcopy(ptr, second, secondLen);

        for (addr_size i = 0; i < firstLen; i++) {
            CT_CHECK(buffer[i] == first[i]);
        }
        for (addr_size i = firstLen; i < firstLen + secondLen; i++) {
            CT_CHECK(buffer[i] == second[i - firstLen]);
        }
    }

    // Validate more complicated types
    {
        struct BiggerTestType {
            u32 a;
            bool b;
            i64 c;
        };

        constexpr addr_size N = 3;
        BiggerTestType buffer[N] = {
            { 1, true, 2 },
            { 3, false, 4 },
            { 5, false, 5 },
        };

        BiggerTestType cpy[N];
        core::memcopy(cpy, buffer, N);

        for (addr_size i = 0; i < N; i++) {
            CT_CHECK(buffer[i].a == cpy[i].a);
            CT_CHECK(buffer[i].b == cpy[i].b);
            CT_CHECK(buffer[i].c == cpy[i].c);
        }
    }

    return 0;
}

constexpr i32 memsetTest() {
    // Validate sequential calls to set.
    {
        constexpr i32 N = 20;
        for (i32 i = 0; i < N; i++) {
            u8 buf[N] = {};
            core::memset(buf, u8(7), addr_size(i)); // set the first i bytes to 7
            for (i32 j = 0; j < i; j++) {
                // CT_CHECK that the first i bytes are 7
                CT_CHECK(buf[j] == 7);
            }
            for (i32 j = i; j < N; j++) {
                // CT_CHECK that the rest are all zeroes
                CT_CHECK(buf[j] == 0);
            }
        }
    }

    // Validate pointer advance after set.
    {
        constexpr addr_size N = 32;
        char buf[N] = {};
        char* ptr = buf;

        ptr += core::memset(buf, 'a', N / 2);
        ptr += core::memset(ptr, 'b', N / 2);

        for (addr_size i = 0; i < N / 2; i++) {
            CT_CHECK(buf[i] == 'a');
        }
        for (addr_size i = N / 2; i < N; i++) {
            CT_CHECK(buf[i] == 'b');
        }
    }

    {
        struct Complex {
            int x;
            f64 y;
        };
        Complex buf[2];
        Complex value = {1, 3.14};
        core::memset(buf, value, 2);
        CT_CHECK(buf[0].x == 1 && buf[0].y == 3.14);
        CT_CHECK(buf[1].x == 1 && buf[1].y == 3.14);
    }

    {
        struct A {
            i32 a;
            u64 b;
            u8 c;
        };

        A arr[10] = {};

        core::memset(arr, A{ 1, 2, 3 }, 10);

        for (auto& a : arr) {
            CT_CHECK(a.a == 1);
            CT_CHECK(a.b == 2);
            CT_CHECK(a.c == 3);
        }
    }

    return 0;
}

constexpr i32 memcmpWithCStrs() {
    struct TestCase {
        const char* a;
        const char* b;
        addr_size n;
        enum { positive = 1, negative = -1, zero = 0 } expected;
    };

    constexpr TestCase cases[] = {
        { nullptr, nullptr, 0, TestCase::zero },
        { nullptr, "", 0, TestCase::zero },
        { "", nullptr, 0, TestCase::zero },
        { "", "", 0, TestCase::zero },
        { "asdzxcasd", "", 0, TestCase::zero },
        { "abc", "abc", 3, TestCase::zero },
        { "abc", "abd", 3, TestCase::negative },
        { "abd", "abc", 3, TestCase::positive },
        { "abc123", "abc000", 3, TestCase::zero },
        { "abc123", "abc000", 4, TestCase::positive },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        switch (c.expected) {
            case 1:  CT_CHECK(core::memcmp(c.a, c.b, c.n) > 0, cErr);  break;
            case -1: CT_CHECK(core::memcmp(c.a, c.b, c.n) < 0, cErr);  break;
            case 0:  CT_CHECK(core::memcmp(c.a, c.b, c.n) == 0, cErr); break;
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 memcmpTest() {
    i32 a[] = { 1, 2, 3, 4, 5 };
    i32 b[] = { 1, 2, 3, 4 };
    i32 c[] = { 9, 9, 9, 9 };

    constexpr auto cmpFn = [](i32 x, i32 y) -> i32 { return x - y; };

    CT_CHECK(core::memcmp(a, 4, b, 4, cmpFn) == 0);
    CT_CHECK(core::memcmp(a, b, 4, cmpFn) == 0);
    CT_CHECK(core::memcmp(a, 4, b, 5, cmpFn) < 0);
    CT_CHECK(core::memcmp(b, 5, a, 4, cmpFn) > 0);

    CT_CHECK(core::memcmp(a, c, 4, cmpFn) < 0);
    CT_CHECK(core::memcmp(c, a, 4, cmpFn) > 0);

    return 0;
}

constexpr i32 appendTest() {
    i32 a[10] = {};
    i32* ptr = a;

    for (i32 i = 0; i < 10; i++) {
        ptr = core::append(ptr, i);
        for (i32 j = 0; j <= i; j++) {
            CT_CHECK(a[j] == j);
        }
    }

    return 0;
}

i32 ptrDiffTest() {
    char array[10] = {};
    char* a = &array[5];
    char* b = &array[2];
    {
        addr_off diff = core::ptrDiff(a, b);
        CT_CHECK(diff == 3);
    }
    {
        addr_off diff = core::ptrDiff(b, a);
        CT_CHECK(diff == -3);
    }
    return 0;
}

i32 ptrAdvanceTest() {
    char arr[] = { 'a', 'b', 'c', 'd', 'e' };
    constexpr addr_size N = CORE_C_ARRLEN(arr);

    for (addr_size i = 0; i < N; i++) {
        void* a = reinterpret_cast<void*>(&arr[0]);
        a = core::ptrAdvance(a, i * sizeof(char));
        CT_CHECK(reinterpret_cast<char*>(a)[0] == arr[i]);
    }

    return 0;
}

constexpr i32 memoryTests() {
    // Test basic functions
    {
        i32 arr[] = {1, 2, 3, 4, 5};
        addr_size N = CORE_C_ARRLEN(arr);
        core::Memory<i32> m(arr, N);
        CT_CHECK(m.len() == N);
        for (addr_size i = 0; i < N; i++) {
            CT_CHECK(m[i] == arr[i]);
            CT_CHECK(m.at(i) != nullptr && *m.at(i) == arr[i]);
        }
    }

    // Test slice()
    {
        i32 arr[] = { 1, 2, 3, 4, 5 };
        core::Memory<i32> m(arr, CORE_C_ARRLEN(arr));

        struct TestCase {
            addr_size offset;
            addr_size len;
            i32 expected[5];
            addr_size expectedLen;
        };

        constexpr TestCase cases[] = {
            { 0, 0, { 0, 0, 0, 0, 0 }, 0 },
            { 0, 1, { 1, 0, 0, 0, 0 }, 1 },
            { 0, 2, { 1, 2, 0, 0, 0 }, 2 },
            { 0, 3, { 1, 2, 3, 0, 0 }, 3 },
            { 0, 4, { 1, 2, 3, 4, 0 }, 4 },
            { 0, 5, { 1, 2, 3, 4, 5 }, 5 },

            { 1, 0, { 0, 0, 0, 0, 0 }, 0 },
            { 1, 1, { 2, 0, 0, 0, 0 }, 1 },
            { 1, 2, { 2, 3, 0, 0, 0 }, 2 },
            { 1, 3, { 2, 3, 4, 0, 0 }, 3 },
            { 1, 4, { 2, 3, 4, 5, 0 }, 4 },

            { 2, 0, { 0, 0, 0, 0, 0 }, 0 },
            { 2, 1, { 3, 0, 0, 0, 0 }, 1 },
            { 2, 2, { 3, 4, 0, 0, 0 }, 2 },
            { 2, 3, { 3, 4, 5, 0, 0 }, 3 },

            { 3, 0, { 0, 0, 0, 0, 0 }, 0 },
            { 3, 1, { 4, 0, 0, 0, 0 }, 1 },
            { 3, 2, { 4, 5, 0, 0, 0 }, 2 },

            { 4, 0, { 0, 0, 0, 0, 0 }, 0 },
            { 4, 1, { 5, 0, 0, 0, 0 }, 1 },

            { 5, 0, { 0, 0, 0, 0, 0 }, 0 },
        };

        i32 ret = core::testing::executeTestTable("slice test case failed at index: ", cases, [&](auto& c, const char* cErr) {
            auto s = m.slice(c.offset, c.len);

            if (c.len == 0) {
                CT_CHECK(s.data() == nullptr, cErr);
            }
            else {
                CT_CHECK(s.data() == arr + c.offset, cErr);
            }
            CT_CHECK(s.len() == c.expectedLen, cErr);

            core::Memory<const i32> actual(s.data(), s.len());
            core::Memory<const i32> expected(c.expected, c.expectedLen);
            CT_CHECK(actual.eq(expected), cErr);

            return 0;
        });
        CT_CHECK(ret == 0);
    }

    // Test eq() and cmp()
    {
        i32 arr1[] = {1, 2, 3};
        i32 arr2[] = {1, 2, 3};
        i32 arr3[] = {1, 2, 4};

        core::Memory<i32> m1(arr1, 3);
        core::Memory<i32> m2(arr2, 3);
        core::Memory<i32> m3(arr3, 3);

        CT_CHECK(m1.eq(m2));
        CT_CHECK(!m1.eq(m3));
        CT_CHECK(m1.cmp(m3) < 0);
        CT_CHECK(m3.cmp(m1) > 0);
    }

    // Test contains()
    {
        i32 arr[] = { 1, 2, 3, 4, 5 };
        core::Memory<i32> m(arr, 4);

        CT_CHECK(m.contains(1));
        CT_CHECK(m.contains(4));
        CT_CHECK(!m.contains(5));
    }

    // Test slice()
    {
        i32 arr[] = { 10, 20, 30, 40, 50 };
        core::Memory<i32> mem(arr, CORE_C_ARRLEN(arr));

        auto tail = mem.slice(1);
        CT_CHECK(tail.data() == arr + 1);
        CT_CHECK(tail.len() == 4);
        tail[0] = 21; // writes through to the original buffer
        CT_CHECK(arr[1] == 21);

        auto middle = mem.slice(2, 2);
        CT_CHECK(middle.data() == arr + 2);
        CT_CHECK(middle.len() == 2);
        CT_CHECK(middle[0] == 30 && middle[1] == 40);
        middle[1] = 41;
        CT_CHECK(arr[3] == 41);

        auto nested = tail.slice(1, 2);
        CT_CHECK(nested.data() == arr + 2);
        CT_CHECK(nested.len() == 2);
        CT_CHECK(nested[0] == 30 && nested[1] == 41);
        nested[0] = 31;
        CT_CHECK(arr[2] == 31);

        auto empty = mem.slice(0, 0);
        CT_CHECK(empty.data() == nullptr);
        CT_CHECK(empty.len() == 0);
    }

    return 0;
}

i32 memoryTestsEdgeCases() {
    // Operations on empty memory objects
    {
        PRAGMA_WARNING_PUSH
        DISABLE_GCC_AND_CLANG_WARNING(-Wself-move)

        auto m1 = core::Memory<char>();
        CT_CHECK(m1.len() == 0);
        CT_CHECK(m1.data() == nullptr);

        m1 = std::move(m1); // move self.
        CT_CHECK(m1.len() == 0);
        CT_CHECK(m1.data() == nullptr);

        auto m2 = core::Memory<char>();
        CT_CHECK(m1.cmp(m2) == 0);
        CT_CHECK(m1.eq(m2) == true);
        CT_CHECK(m1.contains('a') == false);

        PRAGMA_WARNING_POP
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 runMemoryAllocationTest() {
    constexpr addr_size N = 512;
    core::Memory<u8> container[N] = {};
    core::Memory<u8> reallocated[N] = {};

    // Initially allocate everything
    for (addr_size i = 0; i < N; i++) {
        auto m = core::memoryZeroAllocate<u8>(TAllocId, i + 1);
        container[i] = m;
    }

    // Reallocated everything from the container
    for (addr_size i = 0; i < N; i++) {
        reallocated[i] = core::memoryReallocate(std::move(container[i]), container[i].len() * 2, TAllocId);
    }

    // Everything that was moved should be cleared to zero
    for (addr_size i = 0; i < N; i++) {
        CT_CHECK(container[i].data() == nullptr);
        CT_CHECK(container[i].len() == 0);
    }

    // Free everything
    for (addr_size i = 0; i < N; i++) {
        core::memoryFree(std::move(reallocated[i]), TAllocId);
    }

    // Everything that was moved freed should be cleared to zero
    for (addr_size i = 0; i < N; i++) {
        CT_CHECK(reallocated[i].data() == nullptr);
        CT_CHECK(reallocated[i].len() == 0);
    }

    // Reallocating uninitialized should do nothing
    {
        core::Memory<u8> uninitialized;
        auto reallocatedUninitialized = core::memoryReallocate(std::move(uninitialized), 0, TAllocId);
        CT_CHECK(reallocatedUninitialized.data() == uninitialized.data());
        CT_CHECK(reallocatedUninitialized.data() == nullptr);
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 runDynamicMemoryTests() {
    using namespace core::testing;

    TestInfo tInfo = createTestInfoFor(RegisteredAllocators(TAllocId));

    defer { core::getAllocator(TAllocId).clear(); };

    tInfo.name = FN_NAME_TO_CPTR(runMemoryAllocationTest);
    if (runTest(tInfo, runMemoryAllocationTest<TAllocId>) != 0) { return -1; }

    return 0;
}

} // namespace

i32 runMemTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(alignTest);
    if (runTest(tInfo, alignTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memswapTest);
    if (runTest(tInfo, memswapTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(swapTest);
    if (runTest(tInfo, swapTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memcopyTest);
    if (runTest(tInfo, memcopyTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memsetTest);
    if (runTest(tInfo, memsetTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memcmpWithCStrs);
    if (runTest(tInfo, memcmpWithCStrs) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memcmpTest);
    if (runTest(tInfo, memcmpTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(appendTest);
    if (runTest(tInfo, appendTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(ptrDiffTest);
    if (runTest(tInfo, ptrDiffTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(ptrAdvanceTest);
    if (runTest(tInfo, ptrAdvanceTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memoryTests);
    if (runTest(tInfo, memoryTests) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memoryTestsEdgeCases);
    if (runTest(tInfo, memoryTestsEdgeCases) != 0) { ret = -1; }

    if (runDynamicMemoryTests<RA_STD_ALLOCATOR_ID>() != 0) { return -1; }
    if (runDynamicMemoryTests<RA_STD_STATS_ALLOCATOR_ID>() != 0) { return -1; }
    if (runDynamicMemoryTests<RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID>() != 0) { return -1; }
    if (runDynamicMemoryTests<RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID>() != 0) { return -1; }

    constexpr u32 BUFFER_SIZE = core::CORE_MEGABYTE;
    char buf[BUFFER_SIZE];
    setBufferForBumpAllocator(buf, BUFFER_SIZE);
    if (runDynamicMemoryTests<RA_BUMP_ALLOCATOR_ID>() != 0) { return -1; }

    setBlockSizeForArenaAllocator(1024);
    if (runDynamicMemoryTests<RA_ARENA_ALLOCATOR_ID>() != 0) { return -1; }

    return ret;
}

constexpr i32 runCompiletimeMemTestsSuite() {
    RunTestCompileTime(alignTest);
    RunTestCompileTime(memswapTest);
    RunTestCompileTime(swapTest);
    RunTestCompileTime(memcopyTest);
    RunTestCompileTime(memsetTest);
    RunTestCompileTime(memcmpWithCStrs);
    RunTestCompileTime(memcmpTest);
    RunTestCompileTime(appendTest);

    return 0;
}
