#include "t-index.h"

// FIXME:
// 1. Test Errors
// 2. Test edge cases for all basic types.
// 3. Test long formats.

i32 basicFormatTest() {
    constexpr addr_size BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    core::memset(buffer, 9, BUFFER_SIZE);
    {
        auto res = core::format(buffer, BUFFER_SIZE, "No arguments test");
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == core::cstrLen("No arguments test"));
    }

    core::memset(buffer, 9, BUFFER_SIZE);
    {
        f32 a = 78.456113f;
        i8 b = -41;
        u64 c = 21512351232245;
        const char* d = "some text for testing";
        auto res = core::format(buffer, BUFFER_SIZE,
                                "f32 a = {}, i8 b = {}, u64 c = {}, char* d = {}",
                                a, b, c, d);
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() == 88);
    }

    return 0;
}

i32 runFormatTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(basicFormatTest);
    if (runTest(tInfo, basicFormatTest) != 0) { ret = -1; }

    return ret;
}
