#include "../t-index.h"

i32 getCurrentUnixTimestampTest() {
    auto v = core::Unpack(core::getCurrentUnixTimestampMs());
    CT_CHECK(v > 0);
    return 0;
}

i32 runPltTimeTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(getCurrentUnixTimestampTest);
    if (runTest(tInfo, getCurrentUnixTimestampTest) != 0) { ret = -1; }

    return ret;
}
