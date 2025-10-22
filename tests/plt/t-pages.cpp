#include "../t-index.h"

i32 getTheSystemPageSizeTest() {
    addr_size pageSize = core::getPageSize();
    CT_CHECK(pageSize > 0);
    return 0;
}

i32 allocateAndFreePagesTest() {
    constexpr addr_size pageCount = 20;
    void* addr = nullptr;
    {
        auto res = core::allocPages(pageCount);
        CT_CHECK(!res.hasErr());
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() != nullptr);
        addr = res.value();
    }
    {
        auto res = core::freePages(addr, pageCount);
        CT_CHECK(!res.hasErr());
    }

    return 0;
}

i32 runPltPagesTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(getTheSystemPageSizeTest);
    if(runTest(tInfo, getTheSystemPageSizeTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(allocateAndFreePagesTest);
    if(runTest(tInfo, allocateAndFreePagesTest) != 0) { ret = -1; }

    return ret;
}
