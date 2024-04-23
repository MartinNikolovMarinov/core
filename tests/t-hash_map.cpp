#include "t-index.h"

// ----------------------------------------------- Begin Helpers -------------------------------------------------------


template <typename M, typename K, typename V>
inline i32 __test_verifyKeyVal(const M& m, const core::tuple<K, V>& kv) {
    auto& key = kv.v1;
    auto& val = kv.v2;

    auto a = m.get(key);
    CT_CHECK(a != nullptr, "Failed to get data for key");
    CT_CHECK(*a == val, "Value at key is incorrect");

    i32 found = 0;
    m.keys([&](const auto& k) -> bool {
        if (core::eq(k, key)) { found++; }
        return true;
    });
    CT_CHECK(found == 1, "Key should be found exactly once");

    found = 0;
    m.values([&](const auto& v) -> bool {
        if (v == val) { found++; }
        return true;
    });
    CT_CHECK(found > 0, "Value should be found at least once");

    return 0;
}

// test verify many key values with variadic
template <typename M, typename... KV>
inline i32 __test_verifyKeyVals(const M& m, const KV&... kv) {
    return (__test_verifyKeyVal(m, kv), ...);
}

template <typename M, typename K>
inline i32 __test_verifyKey(const M& m, const K& key) {
    auto a = m.get(key);
    CT_CHECK(a != nullptr, "Failed to get data for key");

    i32 found = 0;
    m.keys([&](const auto& k) -> bool {
        if (core::eq(k, key)) { found++; }
        return true;
    });
    CT_CHECK(found == 1, "Key should be found exactly once");

    return 0;
}

template <typename M, typename... K>
inline i32 __test_verifyKeys(const M& m, const K&... keys) {
    return (__test_verifyKey(m, keys), ...);
}

template <typename M>
inline i32 __test_verifyNoKeys(const M& m) {
    bool noKeys = true;
    m.keys([&](const auto&) -> bool { noKeys = false; return true; });
    CT_CHECK(noKeys, "Hash map should not have keys");
    return 0;
}

template <typename M>
inline i32 __test_verifyNoValues(const M& m) {
    bool noValues = true;
    m.values([&](const auto&) -> bool { noValues = false; return true; });
    CT_CHECK(noValues, "Hash map should not have values");
    return 0;
}

// ----------------------------------------------- End Helpers ---------------------------------------------------------

i32 initializeHashMapTest() {
    {
        core::HashMap<i32, i32> m;
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.byteLen() == 0);
        CT_CHECK(m.cap() == 0);
        CT_CHECK(m.byteCap() == 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }
    {
        core::HashMap<core::StrView, core::StrView> m;
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.byteLen() == 0);
        CT_CHECK(m.cap() == 0);
        CT_CHECK(m.byteCap() == 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }
    {
        core::HashMap<i32, i32> m(7);
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.byteLen() == 0);
        CT_CHECK(m.cap() > 0);
        CT_CHECK(m.byteCap() > 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }
    {
        core::HashMap<core::StrView, core::StrView> m(7);
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.byteLen() == 0);
        CT_CHECK(m.cap() > 0);
        CT_CHECK(m.byteCap() > 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }
    {
        core::HashMap<i32, i32> m(0);
        CT_CHECK(m.len() == 0);
        CT_CHECK(m.byteLen() == 0);
        CT_CHECK(m.cap() == 0);
        CT_CHECK(m.byteCap() == 0);
        CT_CHECK(m.empty());
        CT_CHECK(__test_verifyNoKeys(m) == 0);
        CT_CHECK(__test_verifyNoValues(m) == 0);
    }

    return 0;
}

i32 basicPushToHashMapTest() {
    using namespace core::testing;
    using core::sv;
    using core::createTuple;

    {
        core::HashMap<i32, i32> m;

        m.put(1, 1);
        CT_CHECK(m.len() == 1);
        CT_CHECK(__test_verifyKeyVal(m, createTuple(1, 1)) == 0);

        i32 a = 2;
        m.put(a, 1);
        CT_CHECK(m.len() == 2);
        CT_CHECK(m.byteLen() == 2 * core::align(sizeof(i32) + sizeof(i32) + sizeof(bool))); // this test leaks internal details.
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1),
                                         createTuple(2, 1)) == 0);

        i32 b = 9;
        m.put(2, b);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1),
                                         createTuple(2, 9)) == 0);

        m.put(b, a);
        CT_CHECK(m.len() == 3);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1),
                                         createTuple(2, 9),
                                         createTuple(9, 2)) == 0);
    }

    {
        core::HashMap<i32, f32> m;

        m.put(1, 1.0f);
        CT_CHECK(m.len() == 1);
        CT_CHECK(__test_verifyKeyVal(m, createTuple(1, 1.0f)) == 0);

        f32 a = 2.0f;
        m.put(2, a);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1.0f),
                                         createTuple(2, 2.0f)) == 0);

        f32 b = 9.0f;
        m.put(2, b);
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1.0f),
                                         createTuple(2, 9.0f)) == 0);

        i32 c = 9;
        m.put(c, a);
        CT_CHECK(m.len() == 3);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(1, 1.0f),
                                         createTuple(2, 9.0f),
                                         createTuple(9, 2.0f)) == 0);
    }

    {
        core::HashMap<core::StrView, core::StrView> m;

        m.put(sv("1"), "abc"_sv);
        CT_CHECK(m.len() == 1);
        CT_CHECK(__test_verifyKeyVal(m, createTuple(sv("1"), sv("abc"))) == 0);

        auto a = "def"_sv;
        m.put(sv("2"), a);
        CT_CHECK(m.len() == 2);
        CT_CHECK(m.byteLen() == 2 * core::align(sizeof(core::StrView) + sizeof(core::StrView) + sizeof(bool))); // this test leaks internal details.
        CT_CHECK(__test_verifyKeyVals(m, createTuple(sv("1"), "abc"_sv),
                                         createTuple(sv("2"), "def"_sv)) == 0);

        auto b = "2"_sv;
        m.put(b, sv("ghi"));
        CT_CHECK(m.len() == 2);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(sv("1"), "abc"_sv),
                                         createTuple(sv("2"), "ghi"_sv)) == 0);

        m.put(a, b);
        CT_CHECK(m.len() == 3);
        CT_CHECK(__test_verifyKeyVals(m, createTuple(sv("1"), "abc"_sv),
                                         createTuple(sv("2"), "ghi"_sv),
                                         createTuple(sv("def"), "2"_sv)) == 0);
    }

    {
        defer { CT::resetAll(); };

        CT k1, k2, k3, k4;
        k1.a = 1; k2.a = 2; k3.a = 3; k4.a = 4;
        CT::resetAll(); // Reset the counters for the previous line.

        {
            core::HashMap<CT, i32> m;
            CT_CHECK(CT::noCtorsCalled());

            m.put(k1, 9);
            CT_CHECK(CT::copyCtorCalled() == 1);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            m.put(k2, 10);
            CT_CHECK(CT::copyCtorCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            i32 counter = 0;
            i32 err = 0;
            m.values([&](const auto& v) -> bool {
                if (counter == 0 && v != 9) {
                    err = -1;
                    return false;
                }
                if (counter == 1 && v != 10) {
                    err = -2;
                    return false;
                }
                counter++;
                return true;
            });

            CT_CHECK(err == 0);
            CT_CHECK(CT::copyCtorCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
        }
        CT_CHECK(CT::dtorsCalled() == 2);
        CT::resetAll();
    }

    {
        constexpr addr_size N_KEYS = 10;
        i32 keys[N_KEYS] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        f64 values[N_KEYS] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };

        core::HashMap<i32, f64> m;
        for (addr_size i = 0; i < N_KEYS; i++) {
            m.put(keys[i], values[i]);
        }

        CT_CHECK(m.len() == N_KEYS);
        for (addr_size i = 0; i < N_KEYS; i++) {
            CT_CHECK(m.get(keys[i]) != nullptr);
            CT_CHECK(*m.get(keys[i]) == values[i]);
        }
    }

    return 0;
}

i32 basicGetFromHashMapTest() {
    core::HashMap<i32, i32> m;

    CT_CHECK(m.get(1) == nullptr);

    m.put(1, 1);
    CT_CHECK(m.get(1) != nullptr);
    CT_CHECK(*m.get(1) == 1);

    m.put(2, 1);
    CT_CHECK(m.get(2) != nullptr);
    CT_CHECK(*m.get(2) == 1);

    m.put(3, 1);
    CT_CHECK(m.get(3) != nullptr);
    CT_CHECK(*m.get(3) == 1);

    return 0;
}

i32 getWhenHashMapIsFilledToCapacityTest() {
    core::HashMap<i32, i32> m(2);

    m.put(1, 1);
    m.put(2, 1);

    CT_CHECK(m.len() == 2);
    CT_CHECK(m.cap() > 2);
    CT_CHECK(!m.empty());

    CT_CHECK(m.get(1) != nullptr);
    CT_CHECK(m.get(2) != nullptr);
    CT_CHECK(m.get(3) == nullptr); // Make sure these don't loop forever.
    CT_CHECK(m.get(55) == nullptr);

    return 0;
}

i32 removeFromHashMapTest() {
    constexpr i32 N = 5;
    core::HashMap<i32, i32> m(N);

    auto c = m.cap();

    CT_CHECK(!m.remove(1));

    for (i32 i = 0; i < N; i++) {
        m.put(i, i);
    }

    CT_CHECK(c == m.cap()); // no resizing should have happened!

    for (i32 i = 0; i < N; i++) {
        CT_CHECK(m.remove(i));
        CT_CHECK(m.len() == addr_size(N - i - 1));
        CT_CHECK(!m.remove(77));
    }

    CT_CHECK(c == m.cap()); // no resizing should have happened!

    return 0;
}

i32 moveAndCopyHashMapTest() {
    using namespace core::testing;

    {
        auto m = core::createHashMap<i32, f64>();
        m.put(1, 1.0); m.put(2, 2.0); m.put(3, 3.0); m.put(4, 4.0); m.put(5, 5.0);
        m.put(6, 6.0); m.put(7, 7.0); m.put(8, 8.0); m.put(9, 10.0); m.put(10, 10.0);

        bool ok = false;
        core::HashMap<i32, f64> m2 = m.copy();
        auto areTheSame = [](const auto& key, const auto& val, const auto& other) {
            auto a = other.get(key);
            if (a == nullptr || *a != val) {
                return false;
            }
            return true;
        };

        CT_CHECK(m.len() == 10);
        CT_CHECK(m.len() == m2.len());
        CT_CHECK(m.cap() == m2.cap());
        CT_CHECK(m.byteLen() == m2.byteLen());
        CT_CHECK(m.byteCap() == m2.byteCap());

        ok = core::forAll(m, m2, areTheSame);
        CT_CHECK(ok);

        core::HashMap<i32, f64> m3 = std::move(m);

        CT_CHECK(m.len() == 0);
        CT_CHECK(m.empty());
        CT_CHECK(m3.len() == 10);
        CT_CHECK(m3.cap() > 10);

        ok = core::forAll(m3, m2, areTheSame);
        CT_CHECK(ok);
    }

    {
        defer { CT::resetAll(); };

        CT k1, k2; k1.a = 99; k2.a = 199;
        CT v1, v2; v1.a = 77; v2.a = 177;
        CT::resetAll(); // Reset the counters for the previous line.

        {
            core::HashMap<CT, CT> m(2);

            CT_CHECK(!m.remove(k2));
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            CT::resetAll();

            m.put(k1, v1);

            CT_CHECK(CT::copyCtorCalled() == 2);
            CT_CHECK(CT::totalCtorsCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            CT::resetAll();

            m.put(k2, std::move(v2));
            CT_CHECK(CT::copyCtorCalled() == 1);
            CT_CHECK(CT::moveCtorCalled() == 1);
            CT_CHECK(CT::totalCtorsCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            CT::resetAll();

            CT_CHECK(m.remove(k2));
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 2); // v2 and k2 destructors should be called.

            CT::resetAll();

            CT_CHECK(!m.remove(k2));
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);

            {
                core::HashMap<CT, CT> m2;
                m2 = std::move(m);
                CT_CHECK(CT::totalCtorsCalled() == 0);
                CT_CHECK(CT::dtorsCalled() == 0);
                CT_CHECK(CT::assignmentsTotalCalled() == 0);
            }
            CT_CHECK(CT::dtorsCalled() == 2);

            CT::resetAll();
        }
        CT_CHECK(CT::dtorsCalled() == 0);
        CT_CHECK(k1.a == 99);
        CT_CHECK(k2.a == 199);
        CT_CHECK(v1.a == 77);
        CT_CHECK(v2.a == CT::defaultValue);

        CT::resetAll();
    }

    return 0;
}

struct VeryPoorlyHashed {
    i32 a;
    i32 b;
    i32 c;
};

namespace core {

template <> addr_size hash(const VeryPoorlyHashed&) {
    return 12;
}

template <> bool eq(const VeryPoorlyHashed& a, const VeryPoorlyHashed& b) {
    return a.a == b.a && a.b == b.b && a.c == b.c;
}

} // namespace core

i32 veryPoorlyHashedKeyInMapTest() {
    constexpr i32 N = 1000;
    core::HashMap<VeryPoorlyHashed, i32> m;

    for (i32 i = 0; i < N; i++) {
        m.put({ i, i * 2, i * 3 }, i);
    }

    CT_CHECK(m.len() == N);

    for (i32 i = 0; i < N; i++) {
        CT_CHECK(m.contains({ i, i * 2, i * 3 }));
    }

    CT_CHECK(m.len() == N);

    return 0;
}

i32 earlyStopIterationsTest() {
    core::HashMap<i32, i32> m(3);
    m.put(1, 1); m.put(2, 1); m.put(3, 1);

    i32 once;

    once = 0;
    m.keys([&](const auto&) -> bool {
        once++;
        return false;
    });
    CT_CHECK(once == 1);

    once = 0;
    m.values([&](const auto&) -> bool {
        once++;
        return false;
    });
    CT_CHECK(once == 1);

    once = 0;
    m.entries([&](const auto&, const auto&) -> bool {
        once++;
        return false;
    });
    CT_CHECK(once == 1);

    return 0;
}

constexpr i32 hashMapTraitsTest() {
    static_assert(std::is_standard_layout_v<core::HashMap<i32, i32>>,
                 "HashMap(i32, i32) should be standard layout");
    static_assert(std::is_standard_layout_v<core::HashMap<core::StrView, core::StrView>>,
                 "HashMap(StrView, StrView) should be standard layout");

    return 0;
}

i32 runHashMapTestsSuite() {
    using namespace core::testing;

    auto runTests = [] (TestInfo& tInfo, const char* description, i32& retCode) {
        tInfo.description = description;
        tInfo.useAnsiColors = g_useAnsi;

        tInfo.name = FN_NAME_TO_CPTR(initializeHashMapTest);
        if (runTest(tInfo, initializeHashMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(basicPushToHashMapTest);
        if (runTest(tInfo, basicPushToHashMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(basicGetFromHashMapTest);
        if (runTest(tInfo, basicGetFromHashMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(getWhenHashMapIsFilledToCapacityTest);
        if (runTest(tInfo, getWhenHashMapIsFilledToCapacityTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(removeFromHashMapTest);
        if (runTest(tInfo, removeFromHashMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(moveAndCopyHashMapTest);
        if (runTest(tInfo, moveAndCopyHashMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(veryPoorlyHashedKeyInMapTest);
        if (runTest(tInfo, veryPoorlyHashedKeyInMapTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(earlyStopIterationsTest);
        if (runTest(tInfo, earlyStopIterationsTest) != 0) { retCode = -1; }
    };

    i32 ret = 0;
    runForAllGlobalAllocatorVariants(runTests, ret);

    {
        constexpr u32 BUFFER_SIZE = core::CORE_MEGABYTE / 2;
        char buf[BUFFER_SIZE];
        USE_STACK_BASED_BUMP_ALLOCATOR_FOR_BLOCK_SCOPE(buf, BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "STACK BASED BUMP Allocator", ret);
    }

    {
        constexpr u32 BUFFER_SIZE = core::CORE_MEGABYTE / 2; // intentially small to test overflow.
        USE_CUSTOM_ARENA_ALLOCATOR_FOR_FOR_BLOCK_SCOPE(BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "CUSTOM ARENA Allocator", ret);
    }

    return ret;
}

constexpr i32 runCompiletimeHashMapTestsSuite() {
    RunTestCompileTime(hashMapTraitsTest);

    return 0;
}
