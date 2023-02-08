constexpr void least_significatn_N_bits() {
    struct test_case { u8 value; u8 bitSeq; u8 n; bool expected; };

    const i32 testCount = 20;
    test_case testTable[testCount] = {};

    testTable[0] = { 0b0, 0b0, 1, true };
    testTable[1] = { 0b1, 0b1, 1, true };
    testTable[2] = { 0b1, 0b0, 1, false };
    testTable[3] = { 0b0, 0b1, 1, false };

    testTable[4] = { 0b11, 0b11, 2, true };
    testTable[5] = { 0b11, 0b11, 1, false }; // This is undefined behaviour !

    testTable[6] = { 0b100, 0b100, 3, true };
    testTable[7] = { 0b1001, 0b1001, 4, true };
    testTable[8] = { 0b10011, 0b10011, 5, true };
    testTable[9] = { 0b100111, 0b100111, 6, true };
    testTable[10] = { 0b1001110, 0b1001110, 7, true };
    testTable[11] = { 0b10011100, 0b10011100, 8, true };
    testTable[12] = { 0b11111111, 0b11111111, 8, true };
    testTable[13] = { 0b00011111, 0b11111, 5, true };
    testTable[14] = { 0b10000001, 0b10000001, 8, true };
    testTable[15] = { 0b01000010, 0b01000010, 8, true };
    testTable[16] = { 0b01010101, 0b01010101, 8, true };

    testTable[17] = { 0b11111111, 0b11101111, 8, false };
    testTable[18] = { 0b10101111, 0b11101111, 8, false };
    testTable[19] = { 0b11111000, 0b11111, 5, false };

    for (i32 i = 0; i < testCount; i++) {
        bool got = core::lsn_bits(testTable[i].value, testTable[i].bitSeq, testTable[i].n);
        Assert(got == testTable[i].expected);
    }
}

constexpr void most_sifnificant_n_bists() {
    struct test_case { u8 value; u8 bitSeq; u8 n; bool expected; };

    constexpr i32 testCount = 53;
    test_case testTable[testCount] = {};

    testTable[0] = { 0b10000000, 0b1, 1, true };

    testTable[1] = { 0b11000000, 0b1, 1, true };
    testTable[2] = { 0b11000000, 0b11, 2, true };

    testTable[3] = { 0b11100000, 0b1, 1, true };
    testTable[4] = { 0b11100000, 0b11, 2, true };
    testTable[5] = { 0b11100000, 0b111, 3, true };

    testTable[6] = { 0b11110000, 0b1, 1, true };
    testTable[7] = { 0b11110000, 0b11, 2, true };
    testTable[8] = { 0b11110000, 0b111, 3, true };
    testTable[9] = { 0b11110000, 0b1111, 4, true };

    testTable[10] = { 0b11111000, 0b1, 1, true };
    testTable[11] = { 0b11111000, 0b11, 2, true };
    testTable[12] = { 0b11111000, 0b111, 3, true };
    testTable[13] = { 0b11111000, 0b1111, 4, true };
    testTable[14] = { 0b11111000, 0b11111, 5, true };

    testTable[15] = { 0b11111100, 0b1, 1, true };
    testTable[16] = { 0b11111100, 0b11, 2, true };
    testTable[17] = { 0b11111100, 0b111, 3, true };
    testTable[18] = { 0b11111100, 0b1111, 4, true };
    testTable[19] = { 0b11111100, 0b11111, 5, true };
    testTable[20] = { 0b11111100, 0b111111, 6, true };

    testTable[21] = { 0b11111110, 0b1, 1, true };
    testTable[22] = { 0b11111110, 0b11, 2, true };
    testTable[23] = { 0b11111110, 0b111, 3, true };
    testTable[24] = { 0b11111110, 0b1111, 4, true };
    testTable[25] = { 0b11111110, 0b11111, 5, true };
    testTable[26] = { 0b11111110, 0b111111, 6, true };
    testTable[27] = { 0b11111110, 0b1111111, 7, true };

    testTable[28] = { 0b11111111, 0b1, 1, true };
    testTable[29] = { 0b11111111, 0b11, 2, true };
    testTable[30] = { 0b11111111, 0b111, 3, true };
    testTable[31] = { 0b11111111, 0b1111, 4, true };
    testTable[32] = { 0b11111111, 0b11111, 5, true };
    testTable[33] = { 0b11111111, 0b111111, 6, true };
    testTable[34] = { 0b11111111, 0b1111111, 7, true };
    testTable[35] = { 0b11111111, 0b11111111, 8, true };

    testTable[36] = { 0b10001111, 0b1000, 4, true };
    testTable[37] = { 0b10101011, 0b101010, 6, true };
    testTable[38] = { 0b11111101, 0b1111110, 7, true };
    testTable[39] = { 0b11111111, 0b1111, 4, true };

    testTable[40] = { 0b1, 0b1, 1, false };
    testTable[41] = { 0b10, 0b1, 1, false };
    testTable[42] = { 0b100, 0b1, 1, false };
    testTable[43] = { 0b1000, 0b1, 1, false };
    testTable[44] = { 0b10000, 0b1, 1, false };
    testTable[45] = { 0b100000, 0b1, 1, false };
    testTable[46] = { 0b1000000, 0b1, 1, false };

    testTable[47] = { 0b101, 0b101, 3, false };
    testTable[48] = { 0b1010, 0b101, 3, false };
    testTable[49] = { 0b10100, 0b101, 3, false };
    testTable[50] = { 0b101000, 0b101, 3, false };
    testTable[51] = { 0b1010000, 0b101, 3, false };
    testTable[52] = { 0b10100000, 0b101, 3, true };

    for (i32 i = 0; i < testCount; i++) {
        bool got = core::msn_bits(testTable[i].value, testTable[i].bitSeq, testTable[i].n);
        Assert(got == testTable[i].expected);
    }
}

void run_bits_tests_suite() {
    RunTest(least_significatn_N_bits);
    RunTest(most_sifnificant_n_bists);
}
