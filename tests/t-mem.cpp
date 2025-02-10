#include "t-index.h"

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
        i32 arr[] = {1, 2, 3, 4, 5};
        core::Memory<i32> m(arr, 5);

        auto s1 = m.slice(2);
        CT_CHECK(s1.data() == arr + 2);
        CT_CHECK(s1.len() == 3);

        auto s2 = m.slice(2, 2);
        CT_CHECK(s2.data() == arr + 2);
        CT_CHECK(s2.len() == 2);

        for (addr_size i = 0; i < m.len(); i++) {
            {
                auto s = m.slice(0, i);
                for (addr_size j = 0; j < i; j++) {
                    CT_CHECK(s[j] == arr[j]);
                }
            }
            {
                auto s = m.slice(i, m.len() - i);
                for (addr_size j = 0; j < s.len(); j++) {
                    CT_CHECK(s[j] == arr[j + i]);
                }
            }
        }
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

    return 0;
}

i32 runMemTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(alignTest);
    if (runTest(tInfo, alignTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(memswapTest);
    if (runTest(tInfo, memswapTest) != 0) { ret = -1; }
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

    return ret;
}

constexpr i32 runCompiletimeMemTestsSuite() {
    RunTestCompileTime(alignTest);
    RunTestCompileTime(memswapTest);
    RunTestCompileTime(memcopyTest);
    RunTestCompileTime(memsetTest);
    RunTestCompileTime(memcmpWithCStrs);
    RunTestCompileTime(memcmpTest);
    RunTestCompileTime(appendTest);

    return 0;
}
