// TODO: Test invalid input cases!

bool8 ConvertingUTF8SequenceToUTF32Rune_OneBit_Test(constptr char* _tname)
{
    char d[10] = {};
    char d2[10] = {};
    rune r;

    // 0 is NULL, which is the MINUMUM 1 byte encoded character.
    d[0] = 0;
    TryOrFail(r, RuneFromUTF8Sequence((uchar *)d, 1));
    Assert(r == 0);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    // 127 is DELETE, which is the the MAXIMUM 1 byte encoded character.
    d[0] = 127;
    TryOrFail(r, RuneFromUTF8Sequence((uchar *)d, 1));
    Assert(r == 127);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    PrintTestOK(_tname);
    return true;
}

bool8 ConvertingUTF8SequenceToUTF32Rune_TwoBit_Test(constptr char* _tname)
{
    // NOTE: from 128 to 2047 use 2 bytes.

    uchar d[10] = {};
    uchar d2[10] = {};
    rune r;

    // 194, 128 is , which is the MINIMUM 2 byte encoded character.
    d[0] = 194;
    d[1] = 128;
    TryOrFail(r, RuneFromUTF8Sequence((uchar *)d, 2));
    Assert(r == 128);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    // д symbol
    d[0] = 208;
    d[1] = 180;
    TryOrFail(r, RuneFromUTF8Sequence((uchar *)d, 2));
    Assert(r == 1076);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    // 223, 191 is ߿ , which is the MAXIMUM 2 byte encoded character.
    d[0] = 223;
    d[1] = 191;
    TryOrFail(r, RuneFromUTF8Sequence((uchar *)d, 2));
    Assert(r == 2047);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    PrintTestOK(_tname);
    return true;
}

bool8 ConvertingUTF8SequenceToUTF32Rune_ThreeBit_Test(constptr char* _tname)
{
    // NOTE: from 2048 to 65535 use 3 bytes

    uchar d[10] = {};
    uchar d2[10] = {};
    rune r;

    // 224, 160, 128 is ࠀ, which is the MINIMUM 3 byte encoded character!
    d[0] = 224;
    d[1] = 160;
    d[2] = 128;

    TryOrFail(r, RuneFromUTF8Sequence((uchar *)d, 3));
    Assert(r == 2048);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    // 239, 191, 191 is not used, but it is the MAXIMUM possible 3 byte encoding!
    d[0] = 239;
    d[1] = 191;
    d[2] = 191;
    TryOrFail(r, RuneFromUTF8Sequence((uchar *)d, 3));
    Assert(r == 65535);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    PrintTestOK(_tname);
    return true;
}

bool8 ConvertingUTF8SequenceToUTF32Rune_FourBit_Test(constptr char* _tname)
{
    // NOTE: from 65535 to MAX use 4 bytes

    uchar d[10] = {};
    uchar d2[10] = {};
    rune r;

    // 240, 144, 128, 128 is 𐀀, which is the MINUMUM 4 byte encoded character!
    d[0] = 240;
    d[1] = 144;
    d[2] = 128;
    d[3] = 128;
    TryOrFail(r, RuneFromUTF8Sequence((uchar*)d, 4));
    Assert(r == 65536);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    // poop emoji:
    d[0] = 240;
    d[1] = 159;
    d[2] = 146;
    d[3] = 169;
    TryOrFail(r, RuneFromUTF8Sequence((uchar*)d, 4));
    Assert(r == 128169);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    // 247, 191, 191, 191 is not used, but it is the MAXIMUM possible 4 byte encoding!
    d[0] = 247;
    d[1] = 191;
    d[2] = 191;
    d[3] = 191;
    TryOrFail(r, RuneFromUTF8Sequence((uchar*)d, 4));
    Assert(r == 2097151);
    TryOrFailIgnoreOut(RuneToUTF8Sequence(r, (uchar *)d2));
    Assert(CharPtrCmp(d, d2) == 0);

    PrintTestOK(_tname);
    return true;
}

bool8 RunAllRuneTests(constptr char* _tname)
{
    bool8 ret = run_test(ConvertingUTF8SequenceToUTF32Rune_OneBit_Test) &&
                run_test(ConvertingUTF8SequenceToUTF32Rune_TwoBit_Test) &&
                run_test(ConvertingUTF8SequenceToUTF32Rune_ThreeBit_Test) &&
                run_test(ConvertingUTF8SequenceToUTF32Rune_FourBit_Test);
    return ret;
}