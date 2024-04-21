#include "../t-index.h"

i32 getTheSystemPageSizeTest() {
    addr_size pageSize = core::getPageSize();
    CT_CHECK(pageSize > 0);
    return 0;
}

i32 allocateAndFreePagesTest() {
    addr_size pageSize = core::getPageSize();
    constexpr addr_size pageCount = 20;
    void* addr = nullptr;
    {
        auto res = core::allocPages(pageSize * pageCount);
        CT_CHECK(!res.hasErr());
        CT_CHECK(res.hasValue());
        CT_CHECK(res.value() != nullptr);
        addr = res.value();
    }
    {
        auto res = core::freePages(addr, pageSize * pageCount);
        CT_CHECK(!res.hasErr());
    }

    return 0;
}

i32 runPltPagesTestsSuite() {
    RunTest(getTheSystemPageSizeTest);
    RunTest(allocateAndFreePagesTest);

    return 0;
}
