void IntToHexSignedInts() {
    char buf[20];

    core::MemSet(buf, 0, 20);
    core::IntToHex(u8(0), buf);
    Assert(core::CptrCmp(buf, "00") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u8(0xF), buf);
    Assert(core::CptrCmp(buf, "0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_U8, buf);
    Assert(core::CptrCmp(buf, "FF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u16(0), buf);
    Assert(core::CptrCmp(buf, "0000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u16(0xF), buf);
    Assert(core::CptrCmp(buf, "000F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u16(0x1D49), buf);
    Assert(core::CptrCmp(buf, "1D49") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u16(0x0F0F), buf);
    Assert(core::CptrCmp(buf, "0F0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_U16, buf);
    Assert(core::CptrCmp(buf, "FFFF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u32(0), buf);
    Assert(core::CptrCmp(buf, "00000000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u32(0xF), buf);
    Assert(core::CptrCmp(buf, "0000000F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u32(0x1D49), buf);
    Assert(core::CptrCmp(buf, "00001D49") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u32(0x0F0F), buf);
    Assert(core::CptrCmp(buf, "00000F0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u32(0x12345678), buf);
    Assert(core::CptrCmp(buf, "12345678") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_U32, buf);
    Assert(core::CptrCmp(buf, "FFFFFFFF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u64(0), buf);
    Assert(core::CptrCmp(buf, "0000000000000000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u64(0xF), buf);
    Assert(core::CptrCmp(buf, "000000000000000F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u64(0x1D49), buf);
    Assert(core::CptrCmp(buf, "0000000000001D49") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u64(0x0F0F), buf);
    Assert(core::CptrCmp(buf, "0000000000000F0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u64(0x12345678), buf);
    Assert(core::CptrCmp(buf, "0000000012345678") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(u64(0x123456789ABCDEF0), buf);
    Assert(core::CptrCmp(buf, "123456789ABCDEF0") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_U64, buf);
    Assert(core::CptrCmp(buf, "FFFFFFFFFFFFFFFF") == 0);
}

void IntToHexUnsignedInts() {
    char buf[20];

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MIN_I8, buf);
    Assert(core::CptrCmp(buf, "80") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i8(0), buf);
    Assert(core::CptrCmp(buf, "00") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i8(0xF), buf);
    Assert(core::CptrCmp(buf, "0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i8(-1), buf);
    Assert(core::CptrCmp(buf, "FF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_I8, buf);
    Assert(core::CptrCmp(buf, "7F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MIN_I16, buf);
    Assert(core::CptrCmp(buf, "8000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i16(0), buf);
    Assert(core::CptrCmp(buf, "0000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i16(0xF), buf);
    Assert(core::CptrCmp(buf, "000F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i16(0x1D49), buf);
    Assert(core::CptrCmp(buf, "1D49") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i16(0x0F0F), buf);
    Assert(core::CptrCmp(buf, "0F0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i16(-1), buf);
    Assert(core::CptrCmp(buf, "FFFF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i16(-2), buf);
    Assert(core::CptrCmp(buf, "FFFE") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_I16, buf);
    Assert(core::CptrCmp(buf, "7FFF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MIN_I32, buf);
    Assert(core::CptrCmp(buf, "80000000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i32(0), buf);
    Assert(core::CptrCmp(buf, "00000000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i32(0xF), buf);
    Assert(core::CptrCmp(buf, "0000000F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i32(0x1D49), buf);
    Assert(core::CptrCmp(buf, "00001D49") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i32(0x0F0F), buf);
    Assert(core::CptrCmp(buf, "00000F0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i32(0x12345678), buf);
    Assert(core::CptrCmp(buf, "12345678") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i32(-1), buf);
    Assert(core::CptrCmp(buf, "FFFFFFFF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i32(-2), buf);
    Assert(core::CptrCmp(buf, "FFFFFFFE") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_I32, buf);
    Assert(core::CptrCmp(buf, "7FFFFFFF") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MIN_I64, buf);
    Assert(core::CptrCmp(buf, "8000000000000000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i64(0), buf);
    Assert(core::CptrCmp(buf, "0000000000000000") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i64(0xF), buf);
    Assert(core::CptrCmp(buf, "000000000000000F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i64(0x1D49), buf);
    Assert(core::CptrCmp(buf, "0000000000001D49") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i64(0x0F0F), buf);
    Assert(core::CptrCmp(buf, "0000000000000F0F") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i64(0x12345678), buf);
    Assert(core::CptrCmp(buf, "0000000012345678") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(i64(0x123456789ABCDEF0), buf);
    Assert(core::CptrCmp(buf, "123456789ABCDEF0") == 0);

    core::MemSet(buf, 0, 20);
    core::IntToHex(core::MAX_I64, buf);
    Assert(core::CptrCmp(buf, "7FFFFFFFFFFFFFFF") == 0);
}

void RunIntsTestsSuite() {
    RunTest(IntToHexSignedInts);
    RunTest(IntToHexUnsignedInts);
}
