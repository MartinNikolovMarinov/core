#include "core.h"

namespace core
{

namespace
{

static constexpr u32 UTF8_2_BYTE_ENCODING_MASK = 0b11000000;
static constexpr u32 UTF8_3_BYTE_ENCODING_MASK = 0b11100000;
static constexpr u32 UTF8_4_BYTE_ENCODING_MASK = 0b11110000;
static constexpr u32 UTF8_REST_ENCODING_MASK   = 0b00111111;

static constexpr u32 UTF8_MAX_1_BYTE_ENCODING = 127;
static constexpr u32 UTF8_MAX_2_BYTE_ENCODING = 2047;
static constexpr u32 UTF8_MAX_3_BYTE_ENCODING = 65535;
static constexpr u32 UTF8_MAX_4_BYTE_ENCODING = 2097151;

static constexpr u32 UTF8_BYTES_PER_ENCODED_CHUNK = 6;
static constexpr u32 UTF8_CHUNK_DECODING_MASK = 0b111111;

} // namespace

bool IsValidUTF8Encoding(const uchar* utf, i32 len) {
    bool res = false;
    switch(len) {
        case 0:
            res = true;
            break;
        case 1:
            res = MSNBits(utf[0], 0, 1) && (u8)utf[0] <= MAX_I8;
            break;
        case 2:
            res = MSNBits(utf[0], 0b110, 3) &&
                  MSNBits(utf[1], 0b10, 2);
            break;
        case 3:
            res = MSNBits(utf[0], 0b1110, 4) &&
                  MSNBits(utf[1], 0b10, 2) &&
                  MSNBits(utf[2], 0b10, 2);
            break;
        case 4:
            res = MSNBits(utf[0], 0b11110, 5) &&
                  MSNBits(utf[1], 0b10, 2) &&
                  MSNBits(utf[2], 0b10, 2) &&
                  MSNBits(utf[3], 0b10, 2);
            break;
    }
    return res;
}

Tuple<rune, bool> RuneFromBytes(const uchar* utf, i32 len) {
    Assert(utf != nullptr);
    if (IsValidUTF8Encoding(utf, len) == false) {
        return { 0, false };
    }

    rune r = 0;
    const u32 ubpec = UTF8_BYTES_PER_ENCODED_CHUNK;

    switch(len) {
        case 1: {
            r = (rune)utf[0];
            break;
        }
        case 2: {
            u32 b0 = (u32)utf[0] & ~UTF8_2_BYTE_ENCODING_MASK;
            u32 b1 = (u32)utf[1] & UTF8_REST_ENCODING_MASK;
            r = (b0 << ubpec) | (b1);
            break;
        }
        case 3: {
            u32 b0 = (u32)utf[0] & ~UTF8_3_BYTE_ENCODING_MASK;
            u32 b1 = (u32)utf[1] & UTF8_REST_ENCODING_MASK;
            u32 b2 = (u32)utf[2] & UTF8_REST_ENCODING_MASK;
            r = (b0 << (ubpec * 2)) | (b1 << ubpec) | (b2);
            break;
        }
        case 4: {
            u32 b0 = (u32)utf[0] & ~UTF8_4_BYTE_ENCODING_MASK;
            u32 b1 = (u32)utf[1] & UTF8_REST_ENCODING_MASK;
            u32 b2 = (u32)utf[2] & UTF8_REST_ENCODING_MASK;
            u32 b3 = (u32)utf[3] & UTF8_REST_ENCODING_MASK;
            r = (b0 << (ubpec * 3)) | (b1 << (ubpec * 2)) | (b2 << ubpec) | b3;
            break;
        }
    }

    return { r, true };
}


Tuple<i32, bool> RuneToBytes(const rune r, uchar* utf) {
    Assert(utf != nullptr);

    i32 len = 0;
    if (r <= rune(UTF8_MAX_1_BYTE_ENCODING)) len = 1;
    else if (r <= rune(UTF8_MAX_2_BYTE_ENCODING)) len = 2;
    else if (r <= rune(UTF8_MAX_3_BYTE_ENCODING)) len = 3;
    else if (r <= rune(UTF8_MAX_4_BYTE_ENCODING)) len = 4;

    const u32 ubpec = UTF8_BYTES_PER_ENCODED_CHUNK;
    const u32 mask = UTF8_CHUNK_DECODING_MASK;

    switch (len)
    {
        case 1:
            utf[0] = (uchar)r;
            break;
        case 2:
            utf[0] = (uchar)((0b110 << 5)    | (r >> ubpec));
            utf[1] = (uchar)((0b10 << ubpec) | (r & mask));
            break;
        case 3:
            utf[0] = (uchar)((0b1110 << 4)   | (r >> (ubpec * 2)));
            utf[1] = (uchar)((0b10 << ubpec) | ((r >> ubpec) & mask));
            utf[2] = (uchar)((0b10 << ubpec) | (r & mask));
            break;
        case 4:
            utf[0] = (uchar)((0b11110 << 3)  | (r >> (ubpec * 3)));
            utf[1] = (uchar)((0b10 << ubpec) | ((r >> (ubpec * 2)) & mask));
            utf[2] = (uchar)((0b10 << ubpec) | ((r >> ubpec) & mask));
            utf[3] = (uchar)((0b10 << ubpec) | (r & mask));
            break;
    }

    return { len, len > 0 };
}


} // namespace core
