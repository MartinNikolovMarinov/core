// TODO: Test invalid input cases!

void ConvertingUTF8SequenceToUTF32Rune_OneBit_Test()
{
    uchar d[10];
    core::MemSet(d, 0, 10);
    uchar d2[10];
    core::MemSet(d2, 0, 10);
    rune r;
    u32 len;

    // 0 is NULL, which is the MINUMUM 1 byte encoded character.
    d[0] = 0;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 1));
    Assert(r == 0);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 1);
    Assert(core::CptrCmp(d, d2) == 0);

    // 127 is DELETE, which is the the MAXIMUM 1 byte encoded character.
    d[0] = 127;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 1));
    Assert(r == 127);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 1);
    Assert(core::CptrCmp(d, d2) == 0);
}

void ConvertingUTF8SequenceToUTF32Rune_TwoBit_Test()
{
    // NOTE: from 128 to 2047 use 2 bytes.

    uchar d[10];
    core::MemSet(d, 0, 10);
    uchar d2[10];
    core::MemSet(d2, 0, 10);
    rune r;
    u32 len;

    // 194, 128 is , which is the MINIMUM 2 byte encoded character.
    d[0] = 194;
    d[1] = 128;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 2));
    Assert(r == 128);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 2);
    Assert(core::CptrCmp(d, d2) == 0);

    // д symbol
    d[0] = 208;
    d[1] = 180;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 2));
    Assert(r == 1076);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 2);
    Assert(core::CptrCmp(d, d2) == 0);

    // 223, 191 is ߿ , which is the MAXIMUM 2 byte encoded character.
    d[0] = 223;
    d[1] = 191;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 2));
    Assert(r == 2047);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 2);
    Assert(core::CptrCmp(d, d2) == 0);
}

void ConvertingUTF8SequenceToUTF32Rune_ThreeBit_Test()
{
    // NOTE: from 2048 to 65535 use 3 bytes

    uchar d[10];
    core::MemSet(d, 0, 10);
    uchar d2[10];
    core::MemSet(d2, 0, 10);
    rune r;
    u32 len;

    // 224, 160, 128 is ࠀ, which is the MINIMUM 3 byte encoded character!
    d[0] = 224;
    d[1] = 160;
    d[2] = 128;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 3));
    Assert(r == 2048);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 3);
    Assert(core::CptrCmp(d, d2) == 0);

    // 239, 191, 191 is not used, but it is the MAXIMUM possible 3 byte encoding!
    d[0] = 239;
    d[1] = 191;
    d[2] = 191;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 3));
    Assert(r == 65535);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 3);
    Assert(core::CptrCmp(d, d2) == 0);
}

void ConvertingUTF8SequenceToUTF32Rune_FourBit_Test()
{
    // NOTE: from 65535 to MAX use 4 bytes

    uchar d[10];
    core::MemSet(d, 0, 10);
    uchar d2[10];
    core::MemSet(d2, 0, 10);
    rune r;
    u32 len;

    // 240, 144, 128, 128 is 𐀀, which is the MINUMUM 4 byte encoded character!
    d[0] = 240;
    d[1] = 144;
    d[2] = 128;
    d[3] = 128;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 4));
    Assert(r == 65536);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 4);
    Assert(core::CptrCmp(d, d2) == 0);

    // poop emoji:
    d[0] = 240;
    d[1] = 159;
    d[2] = 146;
    d[3] = 169;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 4));
    Assert(r == 128169);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 4);
    Assert(core::CptrCmp(d, d2) == 0);

    // 247, 191, 191, 191 is not used, but it is the MAXIMUM possible 4 byte encoding!
    d[0] = 247;
    d[1] = 191;
    d[2] = 191;
    d[3] = 191;
    r = CheckTuple(core::RuneFromBytes((uchar *)d, 4));
    Assert(r == 2097151);
    len = CheckTuple(core::RuneToBytes(r, (uchar *)d2));
    Assert(len == 4);
    Assert(core::CptrCmp(d, d2) == 0);
}

void RunRuneTestsSuite() {
    RunTest(ConvertingUTF8SequenceToUTF32Rune_OneBit_Test);
    RunTest(ConvertingUTF8SequenceToUTF32Rune_TwoBit_Test);
    RunTest(ConvertingUTF8SequenceToUTF32Rune_ThreeBit_Test);
    RunTest(ConvertingUTF8SequenceToUTF32Rune_FourBit_Test);
}
