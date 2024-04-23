#include "../t-index.h"

constexpr i32 matEqualsTest() {
    auto m1 = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
    auto m2 = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
    auto m3 = core::mat2x2f(core::v(9.0f, 9.0f), core::v(9.0f, 9.0f));
    CT_CHECK(core::mequals(m1, m2));
    CT_CHECK(core::mequals(m2, m1));
    CT_CHECK(m1 == m2);
    CT_CHECK(m2 == m1);
    CT_CHECK(m1 != m3);
    CT_CHECK(m3 != m1);
    CT_CHECK(m2 != m3);
    CT_CHECK(m3 != m2);

    // test safe msafeequals
    auto m4 = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
    auto m5 = core::mat2x2f(core::v(1.000001f, 2.00002f), core::v(3.0003f, 4.000999f));
    CT_CHECK(core::msafeequals(m4, m5, 0.001f));
    CT_CHECK(core::msafeequals(m5, m4, 0.0001f) == false);

    // non-square matrices
    auto m6 = core::mat2x3i(core::v(1, 2, 3), core::v(5, 6, 7));
    auto m7 = core::mat2x3i(core::v(1, 2, 3), core::v(5, 6, 7));
    CT_CHECK(core::mequals(m6, m7));
    CT_CHECK(core::mequals(m7, m6));

    auto m8 = core::mat3x2i(core::v(1, 2), core::v(3, 4), core::v(5, 6));
    auto m9 = core::mat3x2i(core::v(1, 2), core::v(3, 4), core::v(5, 6));
    CT_CHECK(core::mequals(m8, m9));
    CT_CHECK(core::mequals(m9, m8));

    return 0;
}

constexpr i32 mat2xNConstructorsTest() {
    {
        // 2x2
        auto m1 = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[1][0] == 3.0f);
        CT_CHECK(m1[1][1] == 4.0f);

        auto m2 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[1][0] == 3.0f);
        CT_CHECK(m2[1][1] == 4.0f);

        CT_CHECK(m1 == m2);
    }
    {
        // 2x3
        auto m1 = core::mat2x3f(core::v(1.0f, 2.0f, 3.0f), core::v(4.0f, 5.0f, 6.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[0][2] == 3.0f);
        CT_CHECK(m1[1][0] == 4.0f);
        CT_CHECK(m1[1][1] == 5.0f);
        CT_CHECK(m1[1][2] == 6.0f);

        auto m2 = core::mat2x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[0][2] == 3.0f);
        CT_CHECK(m2[1][0] == 4.0f);
        CT_CHECK(m2[1][1] == 5.0f);
        CT_CHECK(m2[1][2] == 6.0f);

        CT_CHECK(m1 == m2);
    }
    {
        // 2x4
        auto m1 = core::mat2x4f(core::v(1.0f, 2.0f, 3.0f, 4.0f),
                                core::v(5.0f, 6.0f, 7.0f, 8.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[0][2] == 3.0f);
        CT_CHECK(m1[0][3] == 4.0f);
        CT_CHECK(m1[1][0] == 5.0f);
        CT_CHECK(m1[1][1] == 6.0f);
        CT_CHECK(m1[1][2] == 7.0f);
        CT_CHECK(m1[1][3] == 8.0f);

        auto m2 = core::mat2x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[0][2] == 3.0f);
        CT_CHECK(m2[0][3] == 4.0f);
        CT_CHECK(m2[1][0] == 5.0f);
        CT_CHECK(m2[1][1] == 6.0f);
        CT_CHECK(m2[1][2] == 7.0f);
        CT_CHECK(m2[1][3] == 8.0f);

        CT_CHECK(m1 == m2);
    }

    return 0;
}

constexpr i32 mat3xNConstructorsTest() {
    {
        // 3x2
        auto m1 = core::mat3x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f), core::v(5.0f, 6.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[1][0] == 3.0f);
        CT_CHECK(m1[1][1] == 4.0f);
        CT_CHECK(m1[2][0] == 5.0f);
        CT_CHECK(m1[2][1] == 6.0f);

        auto m2 = core::mat3x2f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[1][0] == 3.0f);
        CT_CHECK(m2[1][1] == 4.0f);
        CT_CHECK(m2[2][0] == 5.0f);
        CT_CHECK(m2[2][1] == 6.0f);

        CT_CHECK(m1 == m2);
    }
    {
        // 3x3
        auto m1 = core::mat3x3f(core::v(1.0f, 2.0f, 3.0f),
                                core::v(4.0f, 5.0f, 6.0f),
                                core::v(7.0f, 8.0f, 9.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[0][2] == 3.0f);
        CT_CHECK(m1[1][0] == 4.0f);
        CT_CHECK(m1[1][1] == 5.0f);
        CT_CHECK(m1[1][2] == 6.0f);
        CT_CHECK(m1[2][0] == 7.0f);
        CT_CHECK(m1[2][1] == 8.0f);
        CT_CHECK(m1[2][2] == 9.0f);

        auto m2 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[0][2] == 3.0f);
        CT_CHECK(m2[1][0] == 4.0f);
        CT_CHECK(m2[1][1] == 5.0f);
        CT_CHECK(m2[1][2] == 6.0f);
        CT_CHECK(m2[2][0] == 7.0f);
        CT_CHECK(m2[2][1] == 8.0f);
        CT_CHECK(m2[2][2] == 9.0f);

        CT_CHECK(m1 == m2);
    }
    {
        // 3x4
        auto m1 = core::mat3x4f(core::v(1.0f, 2.0f, 3.0f, 4.0f),
                                core::v(5.0f, 6.0f, 7.0f, 8.0f),
                                core::v(9.0f, 10.0f, 11.0f, 12.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[0][2] == 3.0f);
        CT_CHECK(m1[0][3] == 4.0f);
        CT_CHECK(m1[1][0] == 5.0f);
        CT_CHECK(m1[1][1] == 6.0f);
        CT_CHECK(m1[1][2] == 7.0f);
        CT_CHECK(m1[1][3] == 8.0f);
        CT_CHECK(m1[2][0] == 9.0f);
        CT_CHECK(m1[2][1] == 10.0f);
        CT_CHECK(m1[2][2] == 11.0f);
        CT_CHECK(m1[2][3] == 12.0f);

        auto m2 = core::mat3x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[0][2] == 3.0f);
        CT_CHECK(m2[0][3] == 4.0f);
        CT_CHECK(m2[1][0] == 5.0f);
        CT_CHECK(m2[1][1] == 6.0f);
        CT_CHECK(m2[1][2] == 7.0f);
        CT_CHECK(m2[1][3] == 8.0f);
        CT_CHECK(m2[2][0] == 9.0f);
        CT_CHECK(m2[2][1] == 10.0f);
        CT_CHECK(m2[2][2] == 11.0f);
        CT_CHECK(m2[2][3] == 12.0f);

        CT_CHECK(m1 == m2);
    }

    return 0;
}

constexpr i32 mat4xNConstructorsTest() {
    {
        // 4x2
        auto m1 = core::mat4x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f), core::v(5.0f, 6.0f), core::v(7.0f, 8.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[1][0] == 3.0f);
        CT_CHECK(m1[1][1] == 4.0f);
        CT_CHECK(m1[2][0] == 5.0f);
        CT_CHECK(m1[2][1] == 6.0f);
        CT_CHECK(m1[3][0] == 7.0f);
        CT_CHECK(m1[3][1] == 8.0f);

        auto m2 = core::mat4x2f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[1][0] == 3.0f);
        CT_CHECK(m2[1][1] == 4.0f);
        CT_CHECK(m2[2][0] == 5.0f);
        CT_CHECK(m2[2][1] == 6.0f);
        CT_CHECK(m2[3][0] == 7.0f);
        CT_CHECK(m2[3][1] == 8.0f);

        CT_CHECK(m1 == m2);
    }
    {
        // 4x3
        auto m1 = core::mat4x3f(core::v(1.0f, 2.0f, 3.0f), core::v(4.0f, 5.0f, 6.0f), core::v(7.0f, 8.0f, 9.0f), core::v(10.0f, 11.0f, 12.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[0][2] == 3.0f);
        CT_CHECK(m1[1][0] == 4.0f);
        CT_CHECK(m1[1][1] == 5.0f);
        CT_CHECK(m1[1][2] == 6.0f);
        CT_CHECK(m1[2][0] == 7.0f);
        CT_CHECK(m1[2][1] == 8.0f);
        CT_CHECK(m1[2][2] == 9.0f);
        CT_CHECK(m1[3][0] == 10.0f);
        CT_CHECK(m1[3][1] == 11.0f);
        CT_CHECK(m1[3][2] == 12.0f);

        auto m2 = core::mat4x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[0][2] == 3.0f);
        CT_CHECK(m2[1][0] == 4.0f);
        CT_CHECK(m2[1][1] == 5.0f);
        CT_CHECK(m2[1][2] == 6.0f);
        CT_CHECK(m2[2][0] == 7.0f);
        CT_CHECK(m2[2][1] == 8.0f);
        CT_CHECK(m2[2][2] == 9.0f);
        CT_CHECK(m2[3][0] == 10.0f);
        CT_CHECK(m2[3][1] == 11.0f);
        CT_CHECK(m2[3][2] == 12.0f);

        CT_CHECK(m1 == m2);
    }
    {
        // 4x4
        auto m1 = core::mat4x4f(core::v(1.0f, 2.0f, 3.0f, 4.0f),
                                core::v(5.0f, 6.0f, 7.0f, 8.0f),
                                core::v(9.0f, 10.0f, 11.0f, 12.0f),
                                core::v(13.0f, 14.0f, 15.0f, 16.0f));
        CT_CHECK(m1[0][0] == 1.0f);
        CT_CHECK(m1[0][1] == 2.0f);
        CT_CHECK(m1[0][2] == 3.0f);
        CT_CHECK(m1[0][3] == 4.0f);
        CT_CHECK(m1[1][0] == 5.0f);
        CT_CHECK(m1[1][1] == 6.0f);
        CT_CHECK(m1[1][2] == 7.0f);
        CT_CHECK(m1[1][3] == 8.0f);
        CT_CHECK(m1[2][0] == 9.0f);
        CT_CHECK(m1[2][1] == 10.0f);
        CT_CHECK(m1[2][2] == 11.0f);
        CT_CHECK(m1[2][3] == 12.0f);
        CT_CHECK(m1[3][0] == 13.0f);
        CT_CHECK(m1[3][1] == 14.0f);
        CT_CHECK(m1[3][2] == 15.0f);
        CT_CHECK(m1[3][3] == 16.0f);

        auto m2 = core::mat4x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
        CT_CHECK(m2[0][0] == 1.0f);
        CT_CHECK(m2[0][1] == 2.0f);
        CT_CHECK(m2[0][2] == 3.0f);
        CT_CHECK(m2[0][3] == 4.0f);
        CT_CHECK(m2[1][0] == 5.0f);
        CT_CHECK(m2[1][1] == 6.0f);
        CT_CHECK(m2[1][2] == 7.0f);
        CT_CHECK(m2[1][3] == 8.0f);
        CT_CHECK(m2[2][0] == 9.0f);
        CT_CHECK(m2[2][1] == 10.0f);
        CT_CHECK(m2[2][2] == 11.0f);
        CT_CHECK(m2[2][3] == 12.0f);
        CT_CHECK(m2[3][0] == 13.0f);
        CT_CHECK(m2[3][1] == 14.0f);
        CT_CHECK(m2[3][2] == 15.0f);
        CT_CHECK(m2[3][3] == 16.0f);

        CT_CHECK(m1 == m2);
    }

    return 0;
}

constexpr i32 matAddTest() {
    auto testCase = [](auto&& m1, auto&& m2) {
        i32 n = 0;
        for (addr_size i = 0; i < m1.NCol; ++i) {
            for (addr_size j = 0; j < m1.NRow; ++j) {
                m1[i][j] = n++;
                m2[i][j] = n++;
            }
        }

        auto res = m1 + m2;
        for (addr_size i = 0; i < m1.NCol; ++i) {
            for (addr_size j = 0; j < m1.NRow; ++j) {
                CT_CHECK(res[i][j] == m1[i][j] + m2[i][j])
            }
        }

        return 0;
    };

    CT_CHECK(testCase(core::mat2x2i(), core::mat2x2i()) == 0);
    CT_CHECK(testCase(core::mat2x3i(), core::mat2x3i()) == 0);
    CT_CHECK(testCase(core::mat2x4i(), core::mat2x4i()) == 0);
    CT_CHECK(testCase(core::mat3x2i(), core::mat3x2i()) == 0);
    CT_CHECK(testCase(core::mat3x3i(), core::mat3x3i()) == 0);
    CT_CHECK(testCase(core::mat3x4i(), core::mat3x4i()) == 0);
    CT_CHECK(testCase(core::mat4x2i(), core::mat4x2i()) == 0);
    CT_CHECK(testCase(core::mat4x3i(), core::mat4x3i()) == 0);
    CT_CHECK(testCase(core::mat4x4i(), core::mat4x4i()) == 0);

    return 0;
}

constexpr i32 matSubTest() {
    auto testCase = [](auto&& m1, auto&& m2) {
        i32 n = 0;
        for (addr_size i = 0; i < m1.NCol; ++i) {
            for (addr_size j = 0; j < m1.NRow; ++j) {
                m1[i][j] = n++;
                m2[i][j] = n++;
            }
        }

        auto res = m1 - m2;
        for (addr_size i = 0; i < m1.NCol; ++i) {
            for (addr_size j = 0; j < m1.NRow; ++j) {
                CT_CHECK(res[i][j] == m1[i][j] - m2[i][j])
            }
        }

        return 0;
    };

    CT_CHECK(testCase(core::mat2x2i(), core::mat2x2i()) == 0);
    CT_CHECK(testCase(core::mat2x3i(), core::mat2x3i()) == 0);
    CT_CHECK(testCase(core::mat2x4i(), core::mat2x4i()) == 0);
    CT_CHECK(testCase(core::mat3x2i(), core::mat3x2i()) == 0);
    CT_CHECK(testCase(core::mat3x3i(), core::mat3x3i()) == 0);
    CT_CHECK(testCase(core::mat3x4i(), core::mat3x4i()) == 0);
    CT_CHECK(testCase(core::mat4x2i(), core::mat4x2i()) == 0);
    CT_CHECK(testCase(core::mat4x3i(), core::mat4x3i()) == 0);
    CT_CHECK(testCase(core::mat4x4i(), core::mat4x4i()) == 0);

    return 0;
}

constexpr i32 matMulTest() {
    {
        // 2x2 * 2x2 = 2x2
        core::mat2x2i m1(1, 2, 3, 4);
        core::mat2x2i m2(1, 2, 3, 4);
        core::mat2x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 7);
        CT_CHECK(res[0][1] == 10);
        CT_CHECK(res[1][0] == 15);
        CT_CHECK(res[1][1] == 22);
    }
    {
        // 2x2 * 3x2 = 3x2
        core::mat2x2i m1(1, 2, 3, 4);
        core::mat3x2i m2(1, 2, 3, 4, 5, 6);
        core::mat3x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 7);
        CT_CHECK(res[0][1] == 10);
        CT_CHECK(res[1][0] == 15);
        CT_CHECK(res[1][1] == 22);
        CT_CHECK(res[2][0] == 23);
        CT_CHECK(res[2][1] == 34);
    }
    {
        // 2x2 * 4x2 = 4x2
        core::mat2x2i m1(1, 2, 3, 4);
        core::mat4x2i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 7);
        CT_CHECK(res[0][1] == 10);
        CT_CHECK(res[1][0] == 15);
        CT_CHECK(res[1][1] == 22);
        CT_CHECK(res[2][0] == 23);
        CT_CHECK(res[2][1] == 34);
        CT_CHECK(res[3][0] == 31);
        CT_CHECK(res[3][1] == 46);
    }
    {
        // 2x3 * 2x2 = 2x3
        core::mat2x3i m1(1, 2, 3, 4, 5, 6);
        core::mat2x2i m2(1, 2, 3, 4);
        core::mat2x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 9);
        CT_CHECK(res[0][1] == 12);
        CT_CHECK(res[0][2] == 15);
        CT_CHECK(res[1][0] == 19);
        CT_CHECK(res[1][1] == 26);
        CT_CHECK(res[1][2] == 33);
    }
    {
        // 2x3 * 3x2 = 3x3
        core::mat2x3i m1(1, 2, 3, 4, 5, 6);
        core::mat3x2i m2(1, 2, 3, 4, 5, 6);
        core::mat3x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 9);
        CT_CHECK(res[0][1] == 12);
        CT_CHECK(res[0][2] == 15);
        CT_CHECK(res[1][0] == 19);
        CT_CHECK(res[1][1] == 26);
        CT_CHECK(res[1][2] == 33);
        CT_CHECK(res[2][0] == 29);
        CT_CHECK(res[2][1] == 40);
        CT_CHECK(res[2][2] == 51);
    }
    {
        // 2x3 * 4x2 = 4x3
        core::mat2x3i m1(1, 2, 3, 4, 5, 6);
        core::mat4x2i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 9);
        CT_CHECK(res[0][1] == 12);
        CT_CHECK(res[0][2] == 15);
        CT_CHECK(res[1][0] == 19);
        CT_CHECK(res[1][1] == 26);
        CT_CHECK(res[1][2] == 33);
        CT_CHECK(res[2][0] == 29);
        CT_CHECK(res[2][1] == 40);
        CT_CHECK(res[2][2] == 51);
        CT_CHECK(res[3][0] == 39);
        CT_CHECK(res[3][1] == 54);
        CT_CHECK(res[3][2] == 69);
    }
    {
        // 2x4 * 2x2 = 2x4
        core::mat2x4i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x2i m2(1, 2, 3, 4);
        core::mat2x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 11);
        CT_CHECK(res[0][1] == 14);
        CT_CHECK(res[0][2] == 17);
        CT_CHECK(res[0][3] == 20);
        CT_CHECK(res[1][0] == 23);
        CT_CHECK(res[1][1] == 30);
        CT_CHECK(res[1][2] == 37);
        CT_CHECK(res[1][3] == 44);
    }
    {
        // 2x4 * 3x2 = 3x4
        core::mat2x4i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat3x2i m2(1, 2, 3, 4, 5, 6);
        core::mat3x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 11);
        CT_CHECK(res[0][1] == 14);
        CT_CHECK(res[0][2] == 17);
        CT_CHECK(res[0][3] == 20);
        CT_CHECK(res[1][0] == 23);
        CT_CHECK(res[1][1] == 30);
        CT_CHECK(res[1][2] == 37);
        CT_CHECK(res[1][3] == 44);
        CT_CHECK(res[2][0] == 35);
        CT_CHECK(res[2][1] == 46);
        CT_CHECK(res[2][2] == 57);
        CT_CHECK(res[2][3] == 68);
    }
    {
        // 2x4 * 4x2 = 4x4
        core::mat2x4i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x2i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 11);
        CT_CHECK(res[0][1] == 14);
        CT_CHECK(res[0][2] == 17);
        CT_CHECK(res[0][3] == 20);
        CT_CHECK(res[1][0] == 23);
        CT_CHECK(res[1][1] == 30);
        CT_CHECK(res[1][2] == 37);
        CT_CHECK(res[1][3] == 44);
        CT_CHECK(res[2][0] == 35);
        CT_CHECK(res[2][1] == 46);
        CT_CHECK(res[2][2] == 57);
        CT_CHECK(res[2][3] == 68);
        CT_CHECK(res[3][0] == 47);
        CT_CHECK(res[3][1] == 62);
        CT_CHECK(res[3][2] == 77);
        CT_CHECK(res[3][3] == 92);
    }
    {
        // 3x2 * 2x3 = 2x2
        core::mat3x2i m1(1, 2, 3, 4, 5, 6);
        core::mat2x3i m2(1, 2, 3, 4, 5, 6);
        core::mat2x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 22);
        CT_CHECK(res[0][1] == 28);
        CT_CHECK(res[1][0] == 49);
        CT_CHECK(res[1][1] == 64);
    }
    {
        // 3x2 * 3x3 = 3x2
        core::mat3x2i m1(1, 2, 3, 4, 5, 6);
        core::mat3x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 22);
        CT_CHECK(res[0][1] == 28);
        CT_CHECK(res[1][0] == 49);
        CT_CHECK(res[1][1] == 64);
        CT_CHECK(res[2][0] == 76);
        CT_CHECK(res[2][1] == 100);
    }
    {
        // 3x2 * 4x3 = 4x2
        core::mat3x2i m1(1, 2, 3, 4, 5, 6);
        core::mat4x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 22);
        CT_CHECK(res[0][1] == 28);
        CT_CHECK(res[1][0] == 49);
        CT_CHECK(res[1][1] == 64);
        CT_CHECK(res[2][0] == 76);
        CT_CHECK(res[2][1] == 100);
        CT_CHECK(res[3][0] == 103);
        CT_CHECK(res[3][1] == 136);
    }
    {
        // 3x3 * 2x3 = 2x3
        core::mat3x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat2x3i m2(1, 2, 3, 4, 5, 6);
        core::mat2x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 30);
        CT_CHECK(res[0][1] == 36);
        CT_CHECK(res[0][2] == 42);
        CT_CHECK(res[1][0] == 66);
        CT_CHECK(res[1][1] == 81);
        CT_CHECK(res[1][2] == 96);
    }
    {
        // 3x3 * 3x3 = 3x3
        core::mat3x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 30);
        CT_CHECK(res[0][1] == 36);
        CT_CHECK(res[0][2] == 42);
        CT_CHECK(res[1][0] == 66);
        CT_CHECK(res[1][1] == 81);
        CT_CHECK(res[1][2] == 96);
        CT_CHECK(res[2][0] == 102);
        CT_CHECK(res[2][1] == 126);
        CT_CHECK(res[2][2] == 150);
    }
    {
        // 3x3 * 4x3 = 4x3
        core::mat3x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat4x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 30);
        CT_CHECK(res[0][1] == 36);
        CT_CHECK(res[0][2] == 42);
        CT_CHECK(res[1][0] == 66);
        CT_CHECK(res[1][1] == 81);
        CT_CHECK(res[1][2] == 96);
        CT_CHECK(res[2][0] == 102);
        CT_CHECK(res[2][1] == 126);
        CT_CHECK(res[2][2] == 150);
        CT_CHECK(res[3][0] == 138);
        CT_CHECK(res[3][1] == 171);
        CT_CHECK(res[3][2] == 204);
    }
    {
        // 3x4 * 2x3 = 2x4
        core::mat3x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat2x3i m2(1, 2, 3, 4, 5, 6);
        core::mat2x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 38);
        CT_CHECK(res[0][1] == 44);
        CT_CHECK(res[0][2] == 50);
        CT_CHECK(res[0][3] == 56);
        CT_CHECK(res[1][0] == 83);
        CT_CHECK(res[1][1] == 98);
        CT_CHECK(res[1][2] == 113);
        CT_CHECK(res[1][3] == 128);
    }
    {
        // 3x4 * 3x3 = 3x4
        core::mat3x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 38);
        CT_CHECK(res[0][1] == 44);
        CT_CHECK(res[0][2] == 50);
        CT_CHECK(res[0][3] == 56);
        CT_CHECK(res[1][0] == 83);
        CT_CHECK(res[1][1] == 98);
        CT_CHECK(res[1][2] == 113);
        CT_CHECK(res[1][3] == 128);
        CT_CHECK(res[2][0] == 128);
        CT_CHECK(res[2][1] == 152);
        CT_CHECK(res[2][2] == 176);
        CT_CHECK(res[2][3] == 200);
    }
    {
        // 3x4 * 4x3 = 4x4
        core::mat3x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 38);
        CT_CHECK(res[0][1] == 44);
        CT_CHECK(res[0][2] == 50);
        CT_CHECK(res[0][3] == 56);
        CT_CHECK(res[1][0] == 83);
        CT_CHECK(res[1][1] == 98);
        CT_CHECK(res[1][2] == 113);
        CT_CHECK(res[1][3] == 128);
        CT_CHECK(res[2][0] == 128);
        CT_CHECK(res[2][1] == 152);
        CT_CHECK(res[2][2] == 176);
        CT_CHECK(res[2][3] == 200);
        CT_CHECK(res[3][0] == 173);
        CT_CHECK(res[3][1] == 206);
        CT_CHECK(res[3][2] == 239);
        CT_CHECK(res[3][3] == 272);
    }
    {
        // 4x2 * 2x4 = 2x2
        core::mat4x2i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x4i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 50);
        CT_CHECK(res[0][1] == 60);
        CT_CHECK(res[1][0] == 114);
        CT_CHECK(res[1][1] == 140);
    }
    {
        // 4x2 * 3x4 = 3x2
        core::mat4x2i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat3x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 50);
        CT_CHECK(res[0][1] == 60);
        CT_CHECK(res[1][0] == 114);
        CT_CHECK(res[1][1] == 140);
        CT_CHECK(res[2][0] == 178);
        CT_CHECK(res[2][1] == 220);
    }
    {
        // 4x2 * 4x4 = 4x2
        core::mat4x2i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x2i res = m1 * m2;
        CT_CHECK(res[0][0] == 50);
        CT_CHECK(res[0][1] == 60);
        CT_CHECK(res[1][0] == 114);
        CT_CHECK(res[1][1] == 140);
        CT_CHECK(res[2][0] == 178);
        CT_CHECK(res[2][1] == 220);
        CT_CHECK(res[3][0] == 242);
        CT_CHECK(res[3][1] == 300);
    }
    {
        // 4x3 * 2x4 = 2x3
        core::mat4x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat2x4i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 70);
        CT_CHECK(res[0][1] == 80);
        CT_CHECK(res[0][2] == 90);
        CT_CHECK(res[1][0] == 158);
        CT_CHECK(res[1][1] == 184);
        CT_CHECK(res[1][2] == 210);
    }
    {
        // 4x3 * 3x4 = 3x3
        core::mat4x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 70);
        CT_CHECK(res[0][1] == 80);
        CT_CHECK(res[0][2] == 90);
        CT_CHECK(res[1][0] == 158);
        CT_CHECK(res[1][1] == 184);
        CT_CHECK(res[1][2] == 210);
        CT_CHECK(res[2][0] == 246);
        CT_CHECK(res[2][1] == 288);
        CT_CHECK(res[2][2] == 330);
    }
    {
        // 4x3 * 4x4 = 4x3
        core::mat4x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x3i res = m1 * m2;
        CT_CHECK(res[0][0] == 70);
        CT_CHECK(res[0][1] == 80);
        CT_CHECK(res[0][2] == 90);
        CT_CHECK(res[1][0] == 158);
        CT_CHECK(res[1][1] == 184);
        CT_CHECK(res[1][2] == 210);
        CT_CHECK(res[2][0] == 246);
        CT_CHECK(res[2][1] == 288);
        CT_CHECK(res[2][2] == 330);
        CT_CHECK(res[3][0] == 334);
        CT_CHECK(res[3][1] == 392);
        CT_CHECK(res[3][2] == 450);
    }
    {
        // 4x4 * 2x4 = 2x4
        core::mat4x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat2x4i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 90);
        CT_CHECK(res[0][1] == 100);
        CT_CHECK(res[0][2] == 110);
        CT_CHECK(res[0][3] == 120);
        CT_CHECK(res[1][0] == 202);
        CT_CHECK(res[1][1] == 228);
        CT_CHECK(res[1][2] == 254);
        CT_CHECK(res[1][3] == 280);
    }
    {
        // 4x4 * 3x4 = 3x4
        core::mat4x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat3x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 90);
        CT_CHECK(res[0][1] == 100);
        CT_CHECK(res[0][2] == 110);
        CT_CHECK(res[0][3] == 120);
        CT_CHECK(res[1][0] == 202);
        CT_CHECK(res[1][1] == 228);
        CT_CHECK(res[1][2] == 254);
        CT_CHECK(res[1][3] == 280);
        CT_CHECK(res[2][0] == 314);
        CT_CHECK(res[2][1] == 356);
        CT_CHECK(res[2][2] == 398);
        CT_CHECK(res[2][3] == 440);
    }
    {
        // 4x4 * 4x4 = 4x4
        core::mat4x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x4i res = m1 * m2;
        CT_CHECK(res[0][0] == 90);
        CT_CHECK(res[0][1] == 100);
        CT_CHECK(res[0][2] == 110);
        CT_CHECK(res[0][3] == 120);
        CT_CHECK(res[1][0] == 202);
        CT_CHECK(res[1][1] == 228);
        CT_CHECK(res[1][2] == 254);
        CT_CHECK(res[1][3] == 280);
        CT_CHECK(res[2][0] == 314);
        CT_CHECK(res[2][1] == 356);
        CT_CHECK(res[2][2] == 398);
        CT_CHECK(res[2][3] == 440);
        CT_CHECK(res[3][0] == 426);
        CT_CHECK(res[3][1] == 484);
        CT_CHECK(res[3][2] == 542);
        CT_CHECK(res[3][3] == 600);
    }

    return 0;
}

constexpr i32 matMulVectorTest() {
    {
        // 2x2 * 2x1 = 2x1
        core::mat2x2i m1(1, 2, 3, 4);
        core::vec2i v1 = core::v(5, 6);
        core::vec2i ret = m1 * v1;
        CT_CHECK(ret[0] == 23);
        CT_CHECK(ret[1] == 34);
    }
    {
        // 2x3 * 2x1 = 3x1
        core::mat2x3i m1(1, 2, 3, 4, 5, 6);
        core::vec2i v1 = core::v(7, 8);
        core::vec3i ret = m1 * v1;
        CT_CHECK(ret[0] == 39);
        CT_CHECK(ret[1] == 54);
        CT_CHECK(ret[2] == 69);
    }
    {
        // 2x4 * 2x1 = 4x1
        core::mat2x4i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::vec2i v1 = core::v(9, 10);
        core::vec4i ret = m1 * v1;
        CT_CHECK(ret[0] == 59);
        CT_CHECK(ret[1] == 78);
        CT_CHECK(ret[2] == 97);
        CT_CHECK(ret[3] == 116);
    }
    {
        // 3x2 * 3x1 = 2x1
        core::mat3x2i m1(1, 2, 3, 4, 5, 6);
        core::vec3i v1 = core::v(7, 8, 9);
        core::vec2i ret = m1 * v1;
        CT_CHECK(ret[0] == 76);
        CT_CHECK(ret[1] == 100);
    }
    {
        // 3x3 * 3x1 = 3x1
        core::mat3x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::vec3i v1 = core::v(10, 11, 12);
        core::vec3i ret = m1 * v1;
        CT_CHECK(ret[0] == 138);
        CT_CHECK(ret[1] == 171);
        CT_CHECK(ret[2] == 204);
    }
    {
        // 3x4 * 3x1 = 4x1
        core::mat3x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::vec3i v1 = core::v(13, 14, 15);
        core::vec4i ret = m1 * v1;
        CT_CHECK(ret[0] == 218);
        CT_CHECK(ret[1] == 260);
        CT_CHECK(ret[2] == 302);
        CT_CHECK(ret[3] == 344);
    }
    {
        // 4x2 * 4x1 = 2x1
        core::mat4x2i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::vec4i v1 = core::v(9, 10, 11, 12);
        core::vec2i ret = m1 * v1;
        CT_CHECK(ret[0] == 178);
        CT_CHECK(ret[1] == 220);
    }
    {
        // 4x3 * 4x1 = 3x1
        core::mat4x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::vec4i v1 = core::v(13, 14, 15, 16);
        core::vec3i ret = m1 * v1;
        CT_CHECK(ret[0] == 334);
        CT_CHECK(ret[1] == 392);
        CT_CHECK(ret[2] == 450);
    }
    {
        // 4x4 * 4x1 = 4x1
        core::mat4x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::vec4i v1 = core::v(17, 18, 19, 20);
        core::vec4i ret = m1 * v1;
        CT_CHECK(ret[0] == 538);
        CT_CHECK(ret[1] == 612);
        CT_CHECK(ret[2] == 686);
        CT_CHECK(ret[3] == 760);
    }

    return 0;
}

constexpr i32 matDeterminantTest() {
    auto m1 = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
    CT_CHECK(core::mdet(m1) == -2.0f);

    auto m2 = core::mat2x2i(core::v(1, 2), core::v(3, 4));
    CT_CHECK(core::mdet(m2) == -2);

    auto m3 = core::mat3x3f(core::v(1.0f, 1.0f, 1.0f),
                            core::v(0.0f, 1.0f, 0.0f),
                            core::v(1.0f, 0.0f, 1.0f));
    CT_CHECK(core::mdet(m3) == 0.0f);

    auto m4 = core::mat3x3i(core::v(2, 8, 1), core::v(0, 1, 3), core::v(4, 5, 1));
    CT_CHECK(core::mdet(m4) == 64);

    auto m5 = core::mat3x3f(0.0f);
    CT_CHECK(core::mdet(m5) == 0.0f);

    auto m6 = core::mat4x4f(core::v(1.0f, 2.0f, 3.0f, 4.0f),
                            core::v(5.0f, 56.0f, 7.0f, 8.0f),
                            core::v(9.0f, 10.0f, 6.0f, 12.0f),
                            core::v(13.0f, 14.0f, 15.0f, 16.0f));
    CT_CHECK(core::mdet(m6) == 9000.0f);

    return 0;
}

constexpr i32 matIdentityTest() {
    auto m1 = core::mat2x2f::identity();
    CT_CHECK(m1[0][0] == 1.0f);
    CT_CHECK(m1[0][1] == 0.0f);
    CT_CHECK(m1[1][0] == 0.0f);
    CT_CHECK(m1[1][1] == 1.0f);

    auto m2 = core::mat3x3f::identity();
    CT_CHECK(m2[0][0] == 1.0f);
    CT_CHECK(m2[0][1] == 0.0f);
    CT_CHECK(m2[0][2] == 0.0f);
    CT_CHECK(m2[1][0] == 0.0f);
    CT_CHECK(m2[1][1] == 1.0f);
    CT_CHECK(m2[1][2] == 0.0f);
    CT_CHECK(m2[2][0] == 0.0f);
    CT_CHECK(m2[2][1] == 0.0f);
    CT_CHECK(m2[2][2] == 1.0f);

    auto m3 = core::mat4x4f::identity();
    CT_CHECK(m3[0][0] == 1.0f);
    CT_CHECK(m3[0][1] == 0.0f);
    CT_CHECK(m3[0][2] == 0.0f);
    CT_CHECK(m3[0][3] == 0.0f);
    CT_CHECK(m3[1][0] == 0.0f);
    CT_CHECK(m3[1][1] == 1.0f);
    CT_CHECK(m3[1][2] == 0.0f);
    CT_CHECK(m3[1][3] == 0.0f);
    CT_CHECK(m3[2][0] == 0.0f);
    CT_CHECK(m3[2][1] == 0.0f);
    CT_CHECK(m3[2][2] == 1.0f);
    CT_CHECK(m3[2][3] == 0.0f);
    CT_CHECK(m3[3][0] == 0.0f);
    CT_CHECK(m3[3][1] == 0.0f);
    CT_CHECK(m3[3][2] == 0.0f);
    CT_CHECK(m3[3][3] == 1.0f);

    // multiply by identity matrix
    auto m4 = core::mat4x4f(core::v(1.0f, 2.0f, 3.0f, 4.0f),
                            core::v(5.0f, 6.0f, 7.0f, 8.0f),
                            core::v(9.0f, 10.0f, 11.0f, 12.0f),
                            core::v(13.0f, 14.0f, 15.0f, 16.0f));
    CT_CHECK(m4 * core::mat4x4f::identity() == m4);
    CT_CHECK(core::mat4x4f::identity() * m4 == m4);

    auto m5 = core::mat3x3f(core::v(1.0f, 2.0f, 3.0f),
                            core::v(4.0f, 5.0f, 6.0f),
                            core::v(7.0f, 8.0f, 9.0f));
    CT_CHECK(m5 * core::mat3x3f::identity() == m5);
    CT_CHECK(core::mat3x3f::identity() * m5 == m5);

    auto m6 = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
    CT_CHECK(m6 * core::mat2x2f::identity() == m6);
    CT_CHECK(core::mat2x2f::identity() * m6 == m6);

    // multiply by zero matrix
    CT_CHECK(m1 * core::mat2f(0.0f) == core::mat2f(0.0f));
    CT_CHECK(m2 * core::mat3f(0.0f) == core::mat3f(0.0f));
    CT_CHECK(m3 * core::mat4f(0.0f) == core::mat4f(0.0f));

    return 0;
}

constexpr i32 matTransposeTest() {
    auto m1 = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
    auto m1t = core::mat2x2f(core::v(1.0f, 3.0f), core::v(2.0f, 4.0f));
    CT_CHECK(core::mtranspose(m1) == m1t);

    auto m2 = core::mat3x3f(core::v(1.0f, 2.0f, 3.0f),
                            core::v(4.0f, 5.0f, 6.0f),
                            core::v(7.0f, 8.0f, 9.0f));
    auto m2t = core::mat3x3f(core::v(1.0f, 4.0f, 7.0f),
                             core::v(2.0f, 5.0f, 8.0f),
                             core::v(3.0f, 6.0f, 9.0f));
    CT_CHECK(core::mtranspose(m2) == m2t);

    auto m3 = core::mat4x4f(core::v(1.0f, 2.0f, 3.0f, 4.0f),
                            core::v(5.0f, 6.0f, 7.0f, 8.0f),
                            core::v(9.0f, 10.0f, 11.0f, 12.0f),
                            core::v(13.0f, 14.0f, 15.0f, 16.0f));
    auto m3t = core::mat4x4f(core::v(1.0f, 5.0f, 9.0f, 13.0f),
                             core::v(2.0f, 6.0f, 10.0f, 14.0f),
                             core::v(3.0f, 7.0f, 11.0f, 15.0f),
                             core::v(4.0f, 8.0f, 12.0f, 16.0f));
    CT_CHECK(core::mtranspose(m3) == m3t);

    return 0;
}

constexpr i32 matInverseTest() {
    {
        // 2x2
        auto m = core::mat2x2f(core::v(1.0f, 2.0f), core::v(3.0f, 4.0f));
        auto mi = core::mat2x2f(core::v(-2.0f, 1.0f), core::v(1.5f, -0.5f));
        auto res = core::minverse(m);
        CT_CHECK(res == mi);
    }
    {
        // 3x3
        auto m = core::mat3x3f(core::v(1.0f, 2.0f, 3.0f),
                               core::v(3.0f, 2.0f, 1.0f),
                               core::v(2.0f, 1.0f, 3.0f));
        auto mi = core::mat3x3f(core::v(-0.416666687f, 0.25f, 0.333333343f),
                                core::v(0.583333373f, 0.25f, -0.666666687f),
                                core::v(0.0833333358f, -0.25f, 0.333333343f));
        auto res = core::minverse(m);
        CT_CHECK(res == mi);
    }
    {
        // 4x4
        auto m = core::mat4x4f(core::v(1.0f, 4.0f, 5.0f, -1.0f),
                               core::v(-2.0f, 3.0f, -1.0f, 0.0f),
                               core::v(2.0f, 1.0f, 1.0f, 0.0f),
                               core::v(3.0f, -1.0f, 2.0f, 1.0f));
        auto mi = core::mat4x4f(core::v(-0.100000001f, -0.100000001f, 0.600000024f, -0.100000001f),
                                core::v(0.f, 0.25f, 0.25f, 0.f),
                                core::v(0.200000003f, -0.0500000007f, -0.450000018f, 0.200000003f),
                                core::v(-0.100000001f, 0.650000036f, -0.650000036f, 0.900000036f));
        auto res = core::minverse(m);
        CT_CHECK(res == mi);
    }

    return 0;
}

i32 runMatrixTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(matEqualsTest);
    if (runTest(tInfo, matEqualsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(mat2xNConstructorsTest);
    if (runTest(tInfo, mat2xNConstructorsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(mat3xNConstructorsTest);
    if (runTest(tInfo, mat3xNConstructorsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(mat4xNConstructorsTest);
    if (runTest(tInfo, mat4xNConstructorsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matAddTest);
    if (runTest(tInfo, matAddTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matSubTest);
    if (runTest(tInfo, matSubTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matMulTest);
    if (runTest(tInfo, matMulTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matMulVectorTest);
    if (runTest(tInfo, matMulVectorTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matDeterminantTest);
    if (runTest(tInfo, matDeterminantTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matIdentityTest);
    if (runTest(tInfo, matIdentityTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matTransposeTest);
    if (runTest(tInfo, matTransposeTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(matInverseTest);
    if (runTest(tInfo, matInverseTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimeMatrixTestsSuite() {
    RunTestCompileTime(matEqualsTest);
    RunTestCompileTime(mat2xNConstructorsTest);
    RunTestCompileTime(mat3xNConstructorsTest);
    RunTestCompileTime(mat4xNConstructorsTest);
    RunTestCompileTime(matAddTest);
    RunTestCompileTime(matSubTest);
    RunTestCompileTime(matMulTest);
    RunTestCompileTime(matMulVectorTest);
    RunTestCompileTime(matDeterminantTest);
    RunTestCompileTime(matIdentityTest);
    RunTestCompileTime(matTransposeTest);
    RunTestCompileTime(matInverseTest);

    return 0;
}
