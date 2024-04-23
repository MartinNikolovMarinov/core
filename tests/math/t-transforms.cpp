#include "../t-index.h"

constexpr i32 translateTest() {
    core::vec2f v = core::v(1.0f, 2.0f);
    core::vec2f t = core::v(3.0f, 4.0f);
    v = core::translate(v, t);
    CT_CHECK(v == core::v(4.0f, 6.0f));

    v = core::v(1.0f, 2.0f);
    t = core::v(-3.0f, -4.0f);
    v = core::translate(v, t);
    CT_CHECK(v == core::v(-2.0f, -2.0f));

    return 0;
}

constexpr i32 scaleTest() {
    core::vec2f v = core::v(1.0f, 2.0f);
    core::vec2f s = core::v(3.0f, 4.0f);
    v = core::scale(v, s);
    CT_CHECK(v == core::v(3.0f, 8.0f));

    v = core::v(1.0f, 2.0f);
    s = core::v(-3.0f, -4.0f);
    v = core::scale(v, s);
    CT_CHECK(v == core::v(-3.0f, -8.0f));

    v = core::v(1.0f, 2.0f);
    s = core::v(0.0f, 0.0f);
    v = core::scale(v, s);
    CT_CHECK(v == core::v(0.0f, 0.0f));

    return 0;
}

i32 rotate2dTest() {
    constexpr core::vec2f origin = core::v(0.0f, 0.0f);

    core::vec2f v = core::v(1.0f, 2.0f);
    v = core::rotate(v, origin, core::degToRad(0.0f));
    CT_CHECK(v.equals(core::v(1.0f, 2.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotate(v, origin, core::degToRad(90.0f));
    CT_CHECK(v.equals(core::v(-2.0f, 1.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotate(v, origin, core::degToRad(180.0f));
    CT_CHECK(v.equals(core::v(-1.0f, -2.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotate(v, origin, core::degToRad(270.0f));
    CT_CHECK(v.equals(core::v(2.0f, -1.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotate(v, origin, core::degToRad(360.0f));
    CT_CHECK(v.equals(core::v(1.0f, 2.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotateRight(v, origin, core::degToRad(0.0f));
    CT_CHECK(v.equals(core::v(1.0f, 2.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotateRight(v, origin, core::degToRad(90.0f));
    CT_CHECK(v.equals(core::v(2.0f, -1.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotateRight(v, origin, core::degToRad(180.0f));
    CT_CHECK(v.equals(core::v(-1.0f, -2.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotateRight(v, origin, core::degToRad(270.0f));
    CT_CHECK(v.equals(core::v(-2.0f, 1.0f), 0.0001f));

    v = core::v(1.0f, 2.0f);
    v = core::rotateRight(v, origin, core::degToRad(360.0f));
    CT_CHECK(v.equals(core::v(1.0f, 2.0f), 0.0001f));

    constexpr core::vec2f origin2 = core::v(1.0f, 2.0f);

    v = core::v(1.0f, 2.0f);
    v = core::rotate(v, origin2, core::degToRad(0.0f));
    CT_CHECK(v.equals(core::v(1.0f, 2.0f), 0.0001f));

    return 0;
}

i32 runTransformsTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(translateTest);
    if (runTest(tInfo, translateTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(scaleTest);
    if (runTest(tInfo, scaleTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(rotate2dTest);
    if (runTest(tInfo, rotate2dTest) != 0) { ret = -1; }

    return ret;
}

i32 runCompiletimeTransformsTestsSuite() {
    RunTestCompileTime(translateTest);
    RunTestCompileTime(scaleTest);

    return 0;
}
