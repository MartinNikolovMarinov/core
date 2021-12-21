// TODO: Test invalid input cases!
// TODO: Write more tests for wider utf8 character sequences.

bool8 ConvertingUTF8SequenceToUTF32Rune_OneBit_Test(constptr char* tname) {
    char d[10] = {}; // = "д" // 1076
    rune r;

    // 0 is NULL, which is the MINUMUM 1 byte encoded character.
    d[0] = 0;
    r = TryOrFail(RuneFromUTF8Sequence((uchar *)d, 1));
    Assert(r == 0);

    // 127 is DELETE, which is the the MAXIMUM 1 byte encoded character.
    d[0] = 127;
    r = TryOrFail(RuneFromUTF8Sequence((uchar *)d, 1));
    Assert(r == 127);

    PrintTestOK(tname);
    return true;
}

bool8 ConvertingUTF8SequenceToUTF32Rune_TwoBit_Test(constptr char* tname) {
    // NOTE: from 128 to 2047 use 2 bytes.

    char d[10] = {}; // = "д" // 1076
    rune r;

    // 194, 128 is , which is the MINIMUM 2 byte encoded character.
    d[0] = 194;
    d[1] = 128;
    r = TryOrFail(RuneFromUTF8Sequence((uchar *)d, 2));
    Assert(r == 128);

    // д symbol
    d[0] = 208;
    d[1] = 180;
    r = TryOrFail(RuneFromUTF8Sequence((uchar *)d, 2));
    Assert(r == 1076);

    // 223, 191 is ߿ , which is the MAXIMUM 2 byte encoded character.
    d[0] = 223;
    d[1] = 191;
    r = TryOrFail(RuneFromUTF8Sequence((uchar *)d, 2));
    Assert(r == 2047);

    PrintTestOK(tname);
    return true;
}

bool8 ConvertingUTF8SequenceToUTF32Rune_ThreeBit_Test(constptr char* tname) {
    // NOTE: from 2048 to 65535 use 3 bytes

    char d[10] = {}; // = "д" // 1076
    rune r;

    // 224, 160, 128 is ࠀ, which is the MINIMUM 3 byte encoded character!
    d[0] = 224;
    d[1] = 160;
    d[2] = 128;

    r = TryOrFail(RuneFromUTF8Sequence((uchar *)d, 3));
    Assert(r == 2048);

    PrintTestOK(tname);
    return true;
}

bool8 ConvertingUTF8SequenceToUTF32Rune_FourBit_Test(constptr char* tname) {
    // NOTE: from 65535 to MAX use 4 bytes

    char d[10] = {}; // = "д" // 1076
    rune r;

    // poop emoji:
    d[0] = 240;
    d[1] = 159;
    d[2] = 146;
    d[3] = 169;
    r = TryOrFail(RuneFromUTF8Sequence((uchar*)d, 4));
    Assert(r == 128169);

    PrintTestOK(tname);
    return true;
}

bool8 RunAllRuneTests(constptr char* tname) {
    if (run_test(ConvertingUTF8SequenceToUTF32Rune_OneBit_Test) != true) {
        return false;
    }
    if (run_test(ConvertingUTF8SequenceToUTF32Rune_TwoBit_Test) != true) {
        return false;
    }
    if (run_test(ConvertingUTF8SequenceToUTF32Rune_ThreeBit_Test) != true) {
        return false;
    }
    if (run_test(ConvertingUTF8SequenceToUTF32Rune_FourBit_Test) != true) {
        return false;
    }
    return true;
}