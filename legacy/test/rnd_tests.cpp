bool8 RndSignedIntegers_Tests(constptr char* _tname)
{
    const i32 testCount = 12;
    struct { i32 min; i32 max; i32 itterCount; }
    testTable[testCount] = {
        { 0, 1000, 5000 },
        { -1000, 0, 5000 },
        { 10, 11, 5000 },
        { 15, 15, 5000 },
        { -11, -10, 5000 },
        { -19, -19, 5000 },
        { -1000, 1000, 5000 },
        { MIN_I32, MAX_I32, 5000 },
        { MAX_I32 - 500, MAX_I32, 5000 },
        { MIN_I32, MIN_I32 + 1, 5000 },
        { MIN_I32, MIN_I32, 5000 },
        { MAX_I32, MAX_I32, 5000 },
    };

    for (i32 i = 0; i < testCount; i++) {
        for (i32 j = 0; j < testTable[i].itterCount; j++) {
            i32 v = RandI32(testTable[i].min, testTable[i].max);
            Assert(v >= testTable[i].min);
            Assert(v <= testTable[i].max);
        }
    }

    PrintTestOK(_tname);
    return true;
}

bool8 RndRawStr_Tests(constptr char* _tname)
{
    const i32 testCount = 1;
    struct { ptr_size size; i32 itterCount; }
    testTable[testCount] = {
        { 5, 5000 },
    };

    for (i32 i = 0; i < testCount; i++) {
        char buf[5000] = {};
        for (i32 j = 0; j < testTable[i].itterCount; j++) {
            RandRawStr(buf, testTable[i].size);
        }
    }

    PrintTestOK(_tname);
    return true;
}

bool8 RunAllRNDTests(constptr char* _tname)
{
    RandInit();
    bool8 ret = run_test(RndSignedIntegers_Tests) &&
                run_test(RndRawStr_Tests);
    return ret;
}