void rnd_signed_integers() {
    struct TestTable { i32 min; i32 max; i32 itterCount; };

    constexpr i32 testCount = 12;
    TestTable testTable[testCount];

    testTable[0] = { 0, 1000, 5000 };
    testTable[1] = { -1000, 0, 5000 };
    testTable[2] = { 10, 11, 5000 };
    testTable[3] = { 15, 15, 5000 };
    testTable[4] = { -11, -10, 5000 };
    testTable[5] = { -19, -19, 5000 };
    testTable[6] = { -1000, 1000, 5000 };
    testTable[7] = { MIN_I32, MAX_I32, 5000 };
    testTable[8] = { MAX_I32 - 500, MAX_I32, 5000 };
    testTable[9] = { MIN_I32, MIN_I32 + 1, 5000 };
    testTable[10] = { MIN_I32, MIN_I32, 5000 };
    testTable[11] = { MAX_I32, MAX_I32, 5000 };

    for (i32 i = 0; i < testCount; i++) {
        for (i32 j = 0; j < testTable[i].itterCount; j++) {
            i32 v = core::rnd_i32(testTable[i].min, testTable[i].max);
            Assert(v >= testTable[i].min);
            Assert(v <= testTable[i].max);
        }
    }
}

void rnd_raw_str() {
    struct TestTable { ptr_size size; i32 itterCount; };

    constexpr i32 testCount = 1;
    TestTable testTable[testCount];
    testTable[0] = { 5, 5000 };

    for (i32 i = 0; i < testCount; i++) {
        char buf[5000];
        for (i32 j = 0; j < testTable[i].itterCount; j++) {
            core::rnd_cptr(buf, testTable[i].size);
        }
    }
}

void run_rnd_tests_suite() {
    RunTest(rnd_signed_integers);
    RunTest(rnd_raw_str);
}
