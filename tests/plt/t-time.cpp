#include "../t-index.h"

i32 getCurrentUnixTimestampTest() {
    auto v = ValueOrDie(core::getCurrentUnixTimestampMs());
    CT_CHECK(v > 0);
    return 0;
}

i32 runPltTimeTestsSuite() {
    RunTest(getCurrentUnixTimestampTest);

    return 0;
}
