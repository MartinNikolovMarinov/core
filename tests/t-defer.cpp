#include "t-index.h"

i32 deferTest() {
    {
        i32 a = 0;
        {
            CT_CHECK(a == 0);
            defer { a++; };
            CT_CHECK(a == 0);

            {
                CT_CHECK(a == 0);
                defer { a++; };
                CT_CHECK(a == 0);
            }

            CT_CHECK(a == 1);
        }
        CT_CHECK(a == 2);
    }

    return 0;
}

i32 runDeferTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(deferTest);
    if (runTest(tInfo, deferTest) != 0) { ret = -1; }

    return ret;
}
