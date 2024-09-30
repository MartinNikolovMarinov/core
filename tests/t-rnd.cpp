#include "t-index.h"

i32 rndSignedIntegersTest() {
    struct TestCase { i32 min; i32 max; i32 itterCount; };

    constexpr i32 testCount = 12;
    TestCase testTable[testCount];

    testTable[0] = { 0, 1000, 5000 };
    testTable[1] = { -1000, 0, 5000 };
    testTable[2] = { 10, 11, 5000 };
    testTable[3] = { 15, 15, 5000 };
    testTable[4] = { -11, -10, 5000 };
    testTable[5] = { -19, -19, 5000 };
    testTable[6] = { -1000, 1000, 5000 };
    testTable[7] = { core::limitMin<i32>(), core::limitMax<i32>(), 5000 };
    testTable[8] = { core::limitMax<i32>() - 500, core::limitMax<i32>(), 5000 };
    testTable[9] = { core::limitMin<i32>(), core::limitMin<i32>() + 1, 5000 };
    testTable[10] = { core::limitMin<i32>(), core::limitMin<i32>(), 5000 };
    testTable[11] = { core::limitMax<i32>(), core::limitMax<i32>(), 5000 };

    for (i32 i = 0; i < testCount; i++) {
        for (i32 j = 0; j < testTable[i].itterCount; j++) {
            i32 v = core::rndI32(testTable[i].min, testTable[i].max);
            CT_CHECK(v >= testTable[i].min);
            CT_CHECK(v <= testTable[i].max);
        }
    }

    return 0;
}

i32 rndRawStrTest() {
    struct TestCase { addr_size size; i32 itterCount; };

    constexpr i32 testCount = 1;
    TestCase testTable[testCount];
    constexpr i32 max = 5000;
    testTable[0] = { 5, max - 1 }; // -1 for null terminator
    char buf[max];

    for (i32 i = 0; i < testCount; i++) {
        for (i32 j = 0; j < testTable[i].itterCount; j++) {
            core::rndCstr(buf, testTable[i].size);
            buf[max - 1] = '\0';
        }
    }

    return 0;
}

i32 runRndTestsSuite() {
    using namespace core::testing;

    core::rndInit();

    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(rndSignedIntegersTest);
    if (runTest(tInfo, rndSignedIntegersTest) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(rndRawStrTest);
    if (runTest(tInfo, rndRawStrTest) != 0) { return -1; }

    return 0;
}
