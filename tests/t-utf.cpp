#include "t-index.h"

// TODO2: Test invalid input cases!

constexpr i32 convertingUtf8SequenceToUtf32RuneOneBitTest() {
    uchar d[10] = {};
    for (u32 i = 0; i < 10; ++i) d[i] = 0;
    uchar d2[10] = {};
    for (u32 i = 0; i < 10; ++i) d2[i] = 0;

    rune r = 0;
    u32 len = 0;

    // 0 is NULL, which is the MINUMUM 1 byte encoded character.
    d[0] = 0;
    r = core::Unpack(core::runeFromBytes(d, 1));
    CT_CHECK(r == 0);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 1);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    // 127 is DELETE, which is the the MAXIMUM 1 byte encoded character.
    d[0] = 127;
    r = core::Unpack(core::runeFromBytes(d, 1));
    CT_CHECK(r == 127);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 1);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    return 0;
}

constexpr i32 convertingUtf8SequenceToUtf32RuneTwoBitTest() {
    // NOTE: from 128 to 2047 use 2 bytes.

    uchar d[10] = {};
    for (u32 i = 0; i < 10; ++i) d[i] = 0;
    uchar d2[10] = {};
    for (u32 i = 0; i < 10; ++i) d2[i] = 0;
    rune r = 0;
    u32 len = 0;

    // 194, 128 is , which is the MINIMUM 2 byte encoded character.
    d[0] = 194;
    d[1] = 128;
    r = core::Unpack(core::runeFromBytes(d, 2));
    CT_CHECK(r == 128);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 2);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    // д symbol
    d[0] = 208;
    d[1] = 180;
    r = core::Unpack(core::runeFromBytes(d, 2));
    CT_CHECK(r == 1076);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 2);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    // 223, 191 is ߿ , which is the MAXIMUM 2 byte encoded character.
    d[0] = 223;
    d[1] = 191;
    r = core::Unpack(core::runeFromBytes(d, 2));
    CT_CHECK(r == 2047);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 2);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    return 0;
}

constexpr i32 convertingUtf8SequenceToUtf32RuneThreeBitTest() {
    // NOTE: from 2048 to 65535 use 3 bytes

    uchar d[10] = {};
    for (u32 i = 0; i < 10; ++i) d[i] = 0;
    uchar d2[10] = {};
    for (u32 i = 0; i < 10; ++i) d2[i] = 0;
    rune r = 0;
    u32 len = 0;

    // 224, 160, 128 is ࠀ, which is the MINIMUM 3 byte encoded character!
    d[0] = 224;
    d[1] = 160;
    d[2] = 128;
    r = core::Unpack(core::runeFromBytes(d, 3));
    CT_CHECK(r == 2048);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 3);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    // 239, 191, 191 is not used, but it is the MAXIMUM possible 3 byte encoding!
    d[0] = 239;
    d[1] = 191;
    d[2] = 191;
    r = core::Unpack(core::runeFromBytes(d, 3));
    CT_CHECK(r == 65535);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 3);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    return 0;
}

constexpr i32 convertingUtf8SequenceToUtf32RuneFourBitTest() {
    // NOTE: from 65535 to MAX use 4 bytes

    uchar d[10] = {};
    for (u32 i = 0; i < 10; ++i) d[i] = 0;
    uchar d2[10] = {};
    for (u32 i = 0; i < 10; ++i) d2[i] = 0;
    rune r = 0;
    u32 len = 0;

    // 240, 144, 128, 128 is 𐀀, which is the MINUMUM 4 byte encoded character!
    d[0] = 240;
    d[1] = 144;
    d[2] = 128;
    d[3] = 128;
    r = core::Unpack(core::runeFromBytes(d, 4));
    CT_CHECK(r == 65536);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 4);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    // poop emoji:
    d[0] = 240;
    d[1] = 159;
    d[2] = 146;
    d[3] = 169;
    r = core::Unpack(core::runeFromBytes(d, 4));
    CT_CHECK(r == 128169);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 4);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    // 247, 191, 191, 191 is not used, but it is the MAXIMUM possible 4 byte encoding!
    d[0] = 247;
    d[1] = 191;
    d[2] = 191;
    d[3] = 191;
    r = core::Unpack(core::runeFromBytes(d, 4));
    CT_CHECK(r == 2097151);
    len = core::runeToBytes(r, d2);
    CT_CHECK(len == 4);
    CT_CHECK(core::cptrCmp(d, core::cptrLen(d), d2, core::cptrLen(d2)) == 0);

    return 0;
}

i32 runUtfTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(convertingUtf8SequenceToUtf32RuneOneBitTest);
    if (runTest(tInfo, convertingUtf8SequenceToUtf32RuneOneBitTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(convertingUtf8SequenceToUtf32RuneTwoBitTest);
    if (runTest(tInfo, convertingUtf8SequenceToUtf32RuneTwoBitTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(convertingUtf8SequenceToUtf32RuneThreeBitTest);
    if (runTest(tInfo, convertingUtf8SequenceToUtf32RuneThreeBitTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(convertingUtf8SequenceToUtf32RuneFourBitTest);
    if (runTest(tInfo, convertingUtf8SequenceToUtf32RuneFourBitTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeUtfTestsSuite() {
    RunTestCompileTime(convertingUtf8SequenceToUtf32RuneOneBitTest);
    RunTestCompileTime(convertingUtf8SequenceToUtf32RuneTwoBitTest);
    RunTestCompileTime(convertingUtf8SequenceToUtf32RuneThreeBitTest);
    RunTestCompileTime(convertingUtf8SequenceToUtf32RuneFourBitTest);

    return 0;
}
