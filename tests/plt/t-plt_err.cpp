#include "../t-index.h"

i32 pltErrorDescribeWorksTest() {
    char buf[core::MAX_SYSTEM_ERR_MSG_SIZE] = {};
    bool ok = core::pltErrorDescribe(0, buf);
    CT_CHECK(ok, "Platform failed to describe error code 0.");

#if defined(OS_WIN) && OS_WIN == 1
    i32 pltSpecificErrCode = ERROR_FILE_NOT_FOUND;
#elif (defined(OS_LINUX) && OS_LINUX) || (defined(OS_MAC) && OS_MAC == 1)
    i32 pltSpecificErrCode = ENOENT;
#endif

    ok = core::pltErrorDescribe(pltSpecificErrCode, buf);
    CT_CHECK(ok);
    CT_CHECK(core::cstrLen(buf) > 0);

    return 0;
}

i32 runPltErrorTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(pltErrorDescribeWorksTest);
    if (runTest(tInfo, pltErrorDescribeWorksTest) != 0) { ret = -1; }

    return ret;
}
