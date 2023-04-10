void mat_equals() {
    auto m1 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    auto m2 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    auto m3 = core::mat2x2f({ 9.0f, 9.0f }, { 9.0f, 9.0f });
    Assert(core::mequals(m1, m2));
    Assert(core::mequals(m2, m1));
    Assert(m1 == m2);
    Assert(m2 == m1);
    Assert(m1 != m3);
    Assert(m3 != m1);
    Assert(m2 != m3);
    Assert(m3 != m2);

    // test safe msafeequals
    auto m4 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    auto m5 = core::mat2x2f({ 1.000001f, 2.00002f }, { 3.0003f, 4.000999f });
    Assert(core::msafeequals(m4, m5, 0.001f));
    Assert(core::msafeequals(m5, m4, 0.0001f) == false);

    // non-square matrices
    auto m6 = core::mat2x3i({ 1, 2, 3 }, { 5, 6, 7 });
    auto m7 = core::mat2x3i({ 1, 2, 3 }, { 5, 6, 7 });
    Assert(core::mequals(m6, m7));
    Assert(core::mequals(m7, m6));

    auto m8 = core::mat3x2i({ 1, 2 }, { 3, 4 }, { 5, 6 });
    auto m9 = core::mat3x2i({ 1, 2 }, { 3, 4 }, { 5, 6 });
    Assert(core::mequals(m8, m9));
    Assert(core::mequals(m9, m8));
}

void mat2xN_constructors() {
    {
        // 2x2
        auto m1 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[1][0] == 3.0f);
        Assert(m1[1][1] == 4.0f);

        auto m2 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[1][0] == 3.0f);
        Assert(m2[1][1] == 4.0f);

        Assert(m1 == m2);
    }
    {
        // 2x3
        auto m1 = core::mat2x3f({ 1.0f, 2.0f, 3.0f }, { 4.0f, 5.0f, 6.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[0][2] == 3.0f);
        Assert(m1[1][0] == 4.0f);
        Assert(m1[1][1] == 5.0f);
        Assert(m1[1][2] == 6.0f);

        auto m2 = core::mat2x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[0][2] == 3.0f);
        Assert(m2[1][0] == 4.0f);
        Assert(m2[1][1] == 5.0f);
        Assert(m2[1][2] == 6.0f);

        Assert(m1 == m2);
    }
    {
        // 2x4
        auto m1 = core::mat2x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                                { 5.0f, 6.0f, 7.0f, 8.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[0][2] == 3.0f);
        Assert(m1[0][3] == 4.0f);
        Assert(m1[1][0] == 5.0f);
        Assert(m1[1][1] == 6.0f);
        Assert(m1[1][2] == 7.0f);
        Assert(m1[1][3] == 8.0f);

        auto m2 = core::mat2x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[0][2] == 3.0f);
        Assert(m2[0][3] == 4.0f);
        Assert(m2[1][0] == 5.0f);
        Assert(m2[1][1] == 6.0f);
        Assert(m2[1][2] == 7.0f);
        Assert(m2[1][3] == 8.0f);

        Assert(m1 == m2);
    }
}

void mat3xN_constructors() {
    {
        // 3x2
        auto m1 = core::mat3x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f }, { 5.0f, 6.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[1][0] == 3.0f);
        Assert(m1[1][1] == 4.0f);
        Assert(m1[2][0] == 5.0f);
        Assert(m1[2][1] == 6.0f);

        auto m2 = core::mat3x2f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[1][0] == 3.0f);
        Assert(m2[1][1] == 4.0f);
        Assert(m2[2][0] == 5.0f);
        Assert(m2[2][1] == 6.0f);

        Assert(m1 == m2);
    }
    {
        // 3x3
        auto m1 = core::mat3x3f({ 1.0f, 2.0f, 3.0f },
                                { 4.0f, 5.0f, 6.0f },
                                { 7.0f, 8.0f, 9.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[0][2] == 3.0f);
        Assert(m1[1][0] == 4.0f);
        Assert(m1[1][1] == 5.0f);
        Assert(m1[1][2] == 6.0f);
        Assert(m1[2][0] == 7.0f);
        Assert(m1[2][1] == 8.0f);
        Assert(m1[2][2] == 9.0f);

        auto m2 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[0][2] == 3.0f);
        Assert(m2[1][0] == 4.0f);
        Assert(m2[1][1] == 5.0f);
        Assert(m2[1][2] == 6.0f);
        Assert(m2[2][0] == 7.0f);
        Assert(m2[2][1] == 8.0f);
        Assert(m2[2][2] == 9.0f);

        Assert(m1 == m2);
    }
    {
        // 3x4
        auto m1 = core::mat3x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                                { 5.0f, 6.0f, 7.0f, 8.0f },
                                { 9.0f, 10.0f, 11.0f, 12.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[0][2] == 3.0f);
        Assert(m1[0][3] == 4.0f);
        Assert(m1[1][0] == 5.0f);
        Assert(m1[1][1] == 6.0f);
        Assert(m1[1][2] == 7.0f);
        Assert(m1[1][3] == 8.0f);
        Assert(m1[2][0] == 9.0f);
        Assert(m1[2][1] == 10.0f);
        Assert(m1[2][2] == 11.0f);
        Assert(m1[2][3] == 12.0f);

        auto m2 = core::mat3x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[0][2] == 3.0f);
        Assert(m2[0][3] == 4.0f);
        Assert(m2[1][0] == 5.0f);
        Assert(m2[1][1] == 6.0f);
        Assert(m2[1][2] == 7.0f);
        Assert(m2[1][3] == 8.0f);
        Assert(m2[2][0] == 9.0f);
        Assert(m2[2][1] == 10.0f);
        Assert(m2[2][2] == 11.0f);
        Assert(m2[2][3] == 12.0f);

        Assert(m1 == m2);
    }
}

void mat4xN_constructors() {
    {
        // 4x2
        auto m1 = core::mat4x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f }, { 5.0f, 6.0f }, { 7.0f, 8.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[1][0] == 3.0f);
        Assert(m1[1][1] == 4.0f);
        Assert(m1[2][0] == 5.0f);
        Assert(m1[2][1] == 6.0f);
        Assert(m1[3][0] == 7.0f);
        Assert(m1[3][1] == 8.0f);

        auto m2 = core::mat4x2f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[1][0] == 3.0f);
        Assert(m2[1][1] == 4.0f);
        Assert(m2[2][0] == 5.0f);
        Assert(m2[2][1] == 6.0f);
        Assert(m2[3][0] == 7.0f);
        Assert(m2[3][1] == 8.0f);

        Assert(m1 == m2);
    }
    {
        // 4x3
        auto m1 = core::mat4x3f({ 1.0f, 2.0f, 3.0f }, { 4.0f, 5.0f, 6.0f }, { 7.0f, 8.0f, 9.0f }, { 10.0f, 11.0f, 12.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[0][2] == 3.0f);
        Assert(m1[1][0] == 4.0f);
        Assert(m1[1][1] == 5.0f);
        Assert(m1[1][2] == 6.0f);
        Assert(m1[2][0] == 7.0f);
        Assert(m1[2][1] == 8.0f);
        Assert(m1[2][2] == 9.0f);
        Assert(m1[3][0] == 10.0f);
        Assert(m1[3][1] == 11.0f);
        Assert(m1[3][2] == 12.0f);

        auto m2 = core::mat4x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[0][2] == 3.0f);
        Assert(m2[1][0] == 4.0f);
        Assert(m2[1][1] == 5.0f);
        Assert(m2[1][2] == 6.0f);
        Assert(m2[2][0] == 7.0f);
        Assert(m2[2][1] == 8.0f);
        Assert(m2[2][2] == 9.0f);
        Assert(m2[3][0] == 10.0f);
        Assert(m2[3][1] == 11.0f);
        Assert(m2[3][2] == 12.0f);

        Assert(m1 == m2);
    }
    {
        // 4x4
        auto m1 = core::mat4x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                                { 5.0f, 6.0f, 7.0f, 8.0f },
                                { 9.0f, 10.0f, 11.0f, 12.0f },
                                { 13.0f, 14.0f, 15.0f, 16.0f });
        Assert(m1[0][0] == 1.0f);
        Assert(m1[0][1] == 2.0f);
        Assert(m1[0][2] == 3.0f);
        Assert(m1[0][3] == 4.0f);
        Assert(m1[1][0] == 5.0f);
        Assert(m1[1][1] == 6.0f);
        Assert(m1[1][2] == 7.0f);
        Assert(m1[1][3] == 8.0f);
        Assert(m1[2][0] == 9.0f);
        Assert(m1[2][1] == 10.0f);
        Assert(m1[2][2] == 11.0f);
        Assert(m1[2][3] == 12.0f);
        Assert(m1[3][0] == 13.0f);
        Assert(m1[3][1] == 14.0f);
        Assert(m1[3][2] == 15.0f);
        Assert(m1[3][3] == 16.0f);

        auto m2 = core::mat4x4f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
        Assert(m2[0][0] == 1.0f);
        Assert(m2[0][1] == 2.0f);
        Assert(m2[0][2] == 3.0f);
        Assert(m2[0][3] == 4.0f);
        Assert(m2[1][0] == 5.0f);
        Assert(m2[1][1] == 6.0f);
        Assert(m2[1][2] == 7.0f);
        Assert(m2[1][3] == 8.0f);
        Assert(m2[2][0] == 9.0f);
        Assert(m2[2][1] == 10.0f);
        Assert(m2[2][2] == 11.0f);
        Assert(m2[2][3] == 12.0f);
        Assert(m2[3][0] == 13.0f);
        Assert(m2[3][1] == 14.0f);
        Assert(m2[3][2] == 15.0f);
        Assert(m2[3][3] == 16.0f);

        Assert(m1 == m2);
    }
}

template<i32 NCol, i32 NRow>
void __test_fill_mat_with_rnd_ints(core::mat<NCol, NRow, i32>& m) {
    for (i32 i = 0; i < NCol; ++i) {
        for (i32 j = 0; j < NRow; ++j) {
            m[i][j] = core::rnd_i32(-1000, 1000);
        }
    }
}

void mat_add() {
    auto test_case = [](auto&& m1, auto&& m2, i32 i) {
        while(i-- > 0) {
            __test_fill_mat_with_rnd_ints(m1);
            __test_fill_mat_with_rnd_ints(m2);
            auto res = m1 + m2;
            for (i32 i = 0; i < m1.NCol; ++i) {
                for (i32 j = 0; j < m1.NRow; ++j) {
                    Assert(res[i][j] == m1[i][j] + m2[i][j])
                }
            }
        }
    };

    i32 itters = 1000;
    test_case(core::mat2x2i(), core::mat2x2i(), itters);
    test_case(core::mat2x3i(), core::mat2x3i(), itters);
    test_case(core::mat2x4i(), core::mat2x4i(), itters);
    test_case(core::mat3x2i(), core::mat3x2i(), itters);
    test_case(core::mat3x3i(), core::mat3x3i(), itters);
    test_case(core::mat3x4i(), core::mat3x4i(), itters);
    test_case(core::mat4x2i(), core::mat4x2i(), itters);
    test_case(core::mat4x3i(), core::mat4x3i(), itters);
    test_case(core::mat4x4i(), core::mat4x4i(), itters);
}

void mat_sub() {
      auto test_case = [](auto&& m1, auto&& m2, i32 i) {
        while(i-- > 0) {
            __test_fill_mat_with_rnd_ints(m1);
            __test_fill_mat_with_rnd_ints(m2);
            auto res = m1 - m2;
            for (i32 i = 0; i < m1.NCol; ++i) {
                for (i32 j = 0; j < m1.NRow; ++j) {
                    Assert(res[i][j] == m1[i][j] - m2[i][j]);
                }
            }
        }
    };

    i32 itters = 1000;
    test_case(core::mat2x2i(), core::mat2x2i(), itters);
    test_case(core::mat2x3i(), core::mat2x3i(), itters);
    test_case(core::mat2x4i(), core::mat2x4i(), itters);
    test_case(core::mat3x2i(), core::mat3x2i(), itters);
    test_case(core::mat3x3i(), core::mat3x3i(), itters);
    test_case(core::mat3x4i(), core::mat3x4i(), itters);
    test_case(core::mat4x2i(), core::mat4x2i(), itters);
    test_case(core::mat4x3i(), core::mat4x3i(), itters);
    test_case(core::mat4x4i(), core::mat4x4i(), itters);
}

void mat_mul() {
    {
        // 2x2 * 2x2 = 2x2
        core::mat2x2i m1(1, 2, 3, 4);
        core::mat2x2i m2(1, 2, 3, 4);
        core::mat2x2i res = m1 * m2;
        Assert(res[0][0] == 7);
        Assert(res[0][1] == 10);
        Assert(res[1][0] == 15);
        Assert(res[1][1] == 22);
    }
    {
        // 2x2 * 3x2 = 3x2
        core::mat2x2i m1(1, 2, 3, 4);
        core::mat3x2i m2(1, 2, 3, 4, 5, 6);
        core::mat3x2i res = m1 * m2;
        Assert(res[0][0] == 7);
        Assert(res[0][1] == 10);
        Assert(res[1][0] == 15);
        Assert(res[1][1] == 22);
        Assert(res[2][0] == 23);
        Assert(res[2][1] == 34);
    }
    {
        // 2x2 * 4x2 = 4x2
        core::mat2x2i m1(1, 2, 3, 4);
        core::mat4x2i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x2i res = m1 * m2;
        Assert(res[0][0] == 7);
        Assert(res[0][1] == 10);
        Assert(res[1][0] == 15);
        Assert(res[1][1] == 22);
        Assert(res[2][0] == 23);
        Assert(res[2][1] == 34);
        Assert(res[3][0] == 31);
        Assert(res[3][1] == 46);
    }
    {
        // 2x3 * 2x2 = 2x3
        core::mat2x3i m1(1, 2, 3, 4, 5, 6);
        core::mat2x2i m2(1, 2, 3, 4);
        core::mat2x3i res = m1 * m2;
        Assert(res[0][0] == 9);
        Assert(res[0][1] == 12);
        Assert(res[0][2] == 15);
        Assert(res[1][0] == 19);
        Assert(res[1][1] == 26);
        Assert(res[1][2] == 33);
    }
    {
        // 2x3 * 3x2 = 3x3
        core::mat2x3i m1(1, 2, 3, 4, 5, 6);
        core::mat3x2i m2(1, 2, 3, 4, 5, 6);
        core::mat3x3i res = m1 * m2;
        Assert(res[0][0] == 9);
        Assert(res[0][1] == 12);
        Assert(res[0][2] == 15);
        Assert(res[1][0] == 19);
        Assert(res[1][1] == 26);
        Assert(res[1][2] == 33);
        Assert(res[2][0] == 29);
        Assert(res[2][1] == 40);
        Assert(res[2][2] == 51);
    }
    {
        // 2x3 * 4x2 = 4x3
        core::mat2x3i m1(1, 2, 3, 4, 5, 6);
        core::mat4x2i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x3i res = m1 * m2;
        Assert(res[0][0] == 9);
        Assert(res[0][1] == 12);
        Assert(res[0][2] == 15);
        Assert(res[1][0] == 19);
        Assert(res[1][1] == 26);
        Assert(res[1][2] == 33);
        Assert(res[2][0] == 29);
        Assert(res[2][1] == 40);
        Assert(res[2][2] == 51);
        Assert(res[3][0] == 39);
        Assert(res[3][1] == 54);
        Assert(res[3][2] == 69);
    }
    {
        // 2x4 * 2x2 = 2x4
        core::mat2x4i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x2i m2(1, 2, 3, 4);
        core::mat2x4i res = m1 * m2;
        Assert(res[0][0] == 11);
        Assert(res[0][1] == 14);
        Assert(res[0][2] == 17);
        Assert(res[0][3] == 20);
        Assert(res[1][0] == 23);
        Assert(res[1][1] == 30);
        Assert(res[1][2] == 37);
        Assert(res[1][3] == 44);
    }
    {
        // 2x4 * 3x2 = 3x4
        core::mat2x4i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat3x2i m2(1, 2, 3, 4, 5, 6);
        core::mat3x4i res = m1 * m2;
        Assert(res[0][0] == 11);
        Assert(res[0][1] == 14);
        Assert(res[0][2] == 17);
        Assert(res[0][3] == 20);
        Assert(res[1][0] == 23);
        Assert(res[1][1] == 30);
        Assert(res[1][2] == 37);
        Assert(res[1][3] == 44);
        Assert(res[2][0] == 35);
        Assert(res[2][1] == 46);
        Assert(res[2][2] == 57);
        Assert(res[2][3] == 68);
    }
    {
        // 2x4 * 4x2 = 4x4
        core::mat2x4i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x2i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x4i res = m1 * m2;
        Assert(res[0][0] == 11);
        Assert(res[0][1] == 14);
        Assert(res[0][2] == 17);
        Assert(res[0][3] == 20);
        Assert(res[1][0] == 23);
        Assert(res[1][1] == 30);
        Assert(res[1][2] == 37);
        Assert(res[1][3] == 44);
        Assert(res[2][0] == 35);
        Assert(res[2][1] == 46);
        Assert(res[2][2] == 57);
        Assert(res[2][3] == 68);
        Assert(res[3][0] == 47);
        Assert(res[3][1] == 62);
        Assert(res[3][2] == 77);
        Assert(res[3][3] == 92);
    }
    {
        // 3x2 * 2x3 = 2x2
        core::mat3x2i m1(1, 2, 3, 4, 5, 6);
        core::mat2x3i m2(1, 2, 3, 4, 5, 6);
        core::mat2x2i res = m1 * m2;
        Assert(res[0][0] == 22);
        Assert(res[0][1] == 28);
        Assert(res[1][0] == 49);
        Assert(res[1][1] == 64);
    }
    {
        // 3x2 * 3x3 = 3x2
        core::mat3x2i m1(1, 2, 3, 4, 5, 6);
        core::mat3x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x2i res = m1 * m2;
        Assert(res[0][0] == 22);
        Assert(res[0][1] == 28);
        Assert(res[1][0] == 49);
        Assert(res[1][1] == 64);
        Assert(res[2][0] == 76);
        Assert(res[2][1] == 100);
    }
    {
        // 3x2 * 4x3 = 4x2
        core::mat3x2i m1(1, 2, 3, 4, 5, 6);
        core::mat4x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x2i res = m1 * m2;
        Assert(res[0][0] == 22);
        Assert(res[0][1] == 28);
        Assert(res[1][0] == 49);
        Assert(res[1][1] == 64);
        Assert(res[2][0] == 76);
        Assert(res[2][1] == 100);
        Assert(res[3][0] == 103);
        Assert(res[3][1] == 136);
    }
    {
        // 3x3 * 2x3 = 2x3
        core::mat3x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat2x3i m2(1, 2, 3, 4, 5, 6);
        core::mat2x3i res = m1 * m2;
        Assert(res[0][0] == 30);
        Assert(res[0][1] == 36);
        Assert(res[0][2] == 42);
        Assert(res[1][0] == 66);
        Assert(res[1][1] == 81);
        Assert(res[1][2] == 96);
    }
    {
        // 3x3 * 3x3 = 3x3
        core::mat3x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x3i res = m1 * m2;
        Assert(res[0][0] == 30);
        Assert(res[0][1] == 36);
        Assert(res[0][2] == 42);
        Assert(res[1][0] == 66);
        Assert(res[1][1] == 81);
        Assert(res[1][2] == 96);
        Assert(res[2][0] == 102);
        Assert(res[2][1] == 126);
        Assert(res[2][2] == 150);
    }
    {
        // 3x3 * 4x3 = 4x3
        core::mat3x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat4x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x3i res = m1 * m2;
        Assert(res[0][0] == 30);
        Assert(res[0][1] == 36);
        Assert(res[0][2] == 42);
        Assert(res[1][0] == 66);
        Assert(res[1][1] == 81);
        Assert(res[1][2] == 96);
        Assert(res[2][0] == 102);
        Assert(res[2][1] == 126);
        Assert(res[2][2] == 150);
        Assert(res[3][0] == 138);
        Assert(res[3][1] == 171);
        Assert(res[3][2] == 204);
    }
    {
        // 3x4 * 2x3 = 2x4
        core::mat3x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat2x3i m2(1, 2, 3, 4, 5, 6);
        core::mat2x4i res = m1 * m2;
        Assert(res[0][0] == 38);
        Assert(res[0][1] == 44);
        Assert(res[0][2] == 50);
        Assert(res[0][3] == 56);
        Assert(res[1][0] == 83);
        Assert(res[1][1] == 98);
        Assert(res[1][2] == 113);
        Assert(res[1][3] == 128);
    }
    {
        // 3x4 * 3x3 = 3x4
        core::mat3x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9);
        core::mat3x4i res = m1 * m2;
        Assert(res[0][0] == 38);
        Assert(res[0][1] == 44);
        Assert(res[0][2] == 50);
        Assert(res[0][3] == 56);
        Assert(res[1][0] == 83);
        Assert(res[1][1] == 98);
        Assert(res[1][2] == 113);
        Assert(res[1][3] == 128);
        Assert(res[2][0] == 128);
        Assert(res[2][1] == 152);
        Assert(res[2][2] == 176);
        Assert(res[2][3] == 200);
    }
    {
        // 3x4 * 4x3 = 4x4
        core::mat3x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x3i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x4i res = m1 * m2;
        Assert(res[0][0] == 38);
        Assert(res[0][1] == 44);
        Assert(res[0][2] == 50);
        Assert(res[0][3] == 56);
        Assert(res[1][0] == 83);
        Assert(res[1][1] == 98);
        Assert(res[1][2] == 113);
        Assert(res[1][3] == 128);
        Assert(res[2][0] == 128);
        Assert(res[2][1] == 152);
        Assert(res[2][2] == 176);
        Assert(res[2][3] == 200);
        Assert(res[3][0] == 173);
        Assert(res[3][1] == 206);
        Assert(res[3][2] == 239);
        Assert(res[3][3] == 272);
    }
    {
        // 4x2 * 2x4 = 2x2
        core::mat4x2i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x4i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x2i res = m1 * m2;
        Assert(res[0][0] == 50);
        Assert(res[0][1] == 60);
        Assert(res[1][0] == 114);
        Assert(res[1][1] == 140);
    }
    {
        // 4x2 * 3x4 = 3x2
        core::mat4x2i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat3x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x2i res = m1 * m2;
        Assert(res[0][0] == 50);
        Assert(res[0][1] == 60);
        Assert(res[1][0] == 114);
        Assert(res[1][1] == 140);
        Assert(res[2][0] == 178);
        Assert(res[2][1] == 220);
    }
    {
        // 4x2 * 4x4 = 4x2
        core::mat4x2i m1(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat4x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x2i res = m1 * m2;
        Assert(res[0][0] == 50);
        Assert(res[0][1] == 60);
        Assert(res[1][0] == 114);
        Assert(res[1][1] == 140);
        Assert(res[2][0] == 178);
        Assert(res[2][1] == 220);
        Assert(res[3][0] == 242);
        Assert(res[3][1] == 300);
    }
    {
        // 4x3 * 2x4 = 2x3
        core::mat4x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat2x4i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x3i res = m1 * m2;
        Assert(res[0][0] == 70);
        Assert(res[0][1] == 80);
        Assert(res[0][2] == 90);
        Assert(res[1][0] == 158);
        Assert(res[1][1] == 184);
        Assert(res[1][2] == 210);
    }
    {
        // 4x3 * 3x4 = 3x3
        core::mat4x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x3i res = m1 * m2;
        Assert(res[0][0] == 70);
        Assert(res[0][1] == 80);
        Assert(res[0][2] == 90);
        Assert(res[1][0] == 158);
        Assert(res[1][1] == 184);
        Assert(res[1][2] == 210);
        Assert(res[2][0] == 246);
        Assert(res[2][1] == 288);
        Assert(res[2][2] == 330);
    }
    {
        // 4x3 * 4x4 = 4x3
        core::mat4x3i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat4x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x3i res = m1 * m2;
        Assert(res[0][0] == 70);
        Assert(res[0][1] == 80);
        Assert(res[0][2] == 90);
        Assert(res[1][0] == 158);
        Assert(res[1][1] == 184);
        Assert(res[1][2] == 210);
        Assert(res[2][0] == 246);
        Assert(res[2][1] == 288);
        Assert(res[2][2] == 330);
        Assert(res[3][0] == 334);
        Assert(res[3][1] == 392);
        Assert(res[3][2] == 450);
    }
    {
        // 4x4 * 2x4 = 2x4
        core::mat4x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat2x4i m2(1, 2, 3, 4, 5, 6, 7, 8);
        core::mat2x4i res = m1 * m2;
        Assert(res[0][0] == 90);
        Assert(res[0][1] == 100);
        Assert(res[0][2] == 110);
        Assert(res[0][3] == 120);
        Assert(res[1][0] == 202);
        Assert(res[1][1] == 228);
        Assert(res[1][2] == 254);
        Assert(res[1][3] == 280);
    }
    {
        // 4x4 * 3x4 = 3x4
        core::mat4x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat3x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        core::mat3x4i res = m1 * m2;
        Assert(res[0][0] == 90);
        Assert(res[0][1] == 100);
        Assert(res[0][2] == 110);
        Assert(res[0][3] == 120);
        Assert(res[1][0] == 202);
        Assert(res[1][1] == 228);
        Assert(res[1][2] == 254);
        Assert(res[1][3] == 280);
        Assert(res[2][0] == 314);
        Assert(res[2][1] == 356);
        Assert(res[2][2] == 398);
        Assert(res[2][3] == 440);
    }
    {
        // 4x4 * 4x4 = 4x4
        core::mat4x4i m1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x4i m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        core::mat4x4i res = m1 * m2;
        Assert(res[0][0] == 90);
        Assert(res[0][1] == 100);
        Assert(res[0][2] == 110);
        Assert(res[0][3] == 120);
        Assert(res[1][0] == 202);
        Assert(res[1][1] == 228);
        Assert(res[1][2] == 254);
        Assert(res[1][3] == 280);
        Assert(res[2][0] == 314);
        Assert(res[2][1] == 356);
        Assert(res[2][2] == 398);
        Assert(res[2][3] == 440);
        Assert(res[3][0] == 426);
        Assert(res[3][1] == 484);
        Assert(res[3][2] == 542);
        Assert(res[3][3] == 600);
    }
}

void mat_determinant() {
    auto m1 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    Assert(core::mdet(m1) == -2.0f);

    auto m2 = core::mat2x2i({ 1, 2 }, { 3, 4 });
    Assert(core::mdet(m2) == -2);

    auto m3 = core::mat3x3f({ 1.0f, 1.0f, 1.0f },
                            { 0.0f, 1.0f, 0.0f },
                            { 1.0f, 0.0f, 1.0f });
    Assert(core::mdet(m3) == 0.0f);

    auto m4 = core::mat3x3i({ 2, 8, 1 }, { 0, 1, 3 }, { 4, 5, 1 });
    Assert(core::mdet(m4) == 64);

    auto m5 = core::mat3x3f(0.0f);
    Assert(core::mdet(m5) == 0.0f);

    auto m6 = core::mat4x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                            { 5.0f, 56.0f, 7.0f, 8.0f },
                            { 9.0f, 10.0f, 6.0f, 12.0f },
                            { 13.0f, 14.0f, 15.0f, 16.0f });
    Assert(core::mdet(m6) == 9000.0f);
}

void mat_identity() {
    auto m1 = core::mat2x2f::identity();
    Assert(m1[0][0] == 1.0f);
    Assert(m1[0][1] == 0.0f);
    Assert(m1[1][0] == 0.0f);
    Assert(m1[1][1] == 1.0f);

    auto m2 = core::mat3x3f::identity();
    Assert(m2[0][0] == 1.0f);
    Assert(m2[0][1] == 0.0f);
    Assert(m2[0][2] == 0.0f);
    Assert(m2[1][0] == 0.0f);
    Assert(m2[1][1] == 1.0f);
    Assert(m2[1][2] == 0.0f);
    Assert(m2[2][0] == 0.0f);
    Assert(m2[2][1] == 0.0f);
    Assert(m2[2][2] == 1.0f);

    auto m3 = core::mat4x4f::identity();
    Assert(m3[0][0] == 1.0f);
    Assert(m3[0][1] == 0.0f);
    Assert(m3[0][2] == 0.0f);
    Assert(m3[0][3] == 0.0f);
    Assert(m3[1][0] == 0.0f);
    Assert(m3[1][1] == 1.0f);
    Assert(m3[1][2] == 0.0f);
    Assert(m3[1][3] == 0.0f);
    Assert(m3[2][0] == 0.0f);
    Assert(m3[2][1] == 0.0f);
    Assert(m3[2][2] == 1.0f);
    Assert(m3[2][3] == 0.0f);
    Assert(m3[3][0] == 0.0f);
    Assert(m3[3][1] == 0.0f);
    Assert(m3[3][2] == 0.0f);
    Assert(m3[3][3] == 1.0f);

    // multiply by identity matrix
    auto m4 = core::mat4x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                            { 5.0f, 6.0f, 7.0f, 8.0f },
                            { 9.0f, 10.0f, 11.0f, 12.0f },
                            { 13.0f, 14.0f, 15.0f, 16.0f });
    Assert(m4 * core::mat4x4f::identity() == m4);
    Assert(core::mat4x4f::identity() * m4 == m4);

    auto m5 = core::mat3x3f({ 1.0f, 2.0f, 3.0f },
                            { 4.0f, 5.0f, 6.0f },
                            { 7.0f, 8.0f, 9.0f });
    Assert(m5 * core::mat3x3f::identity() == m5);
    Assert(core::mat3x3f::identity() * m5 == m5);

    auto m6 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    Assert(m6 * core::mat2x2f::identity() == m6);
    Assert(core::mat2x2f::identity() * m6 == m6);

    // multiply by zero matrix
    Assert(m1 * core::mat2f(0.0f) == core::mat2f(0.0f));
    Assert(m2 * core::mat3f(0.0f) == core::mat3f(0.0f));
    Assert(m3 * core::mat4f(0.0f) == core::mat4f(0.0f));
}

void mat_transpose() {
    auto m1 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    auto m1t = core::mat2x2f({ 1.0f, 3.0f }, { 2.0f, 4.0f });
    Assert(core::mtranspose(m1) == m1t);

    auto m2 = core::mat3x3f({ 1.0f, 2.0f, 3.0f },
                            { 4.0f, 5.0f, 6.0f },
                            { 7.0f, 8.0f, 9.0f });
    auto m2t = core::mat3x3f({ 1.0f, 4.0f, 7.0f },
                             { 2.0f, 5.0f, 8.0f },
                             { 3.0f, 6.0f, 9.0f });
    Assert(core::mtranspose(m2) == m2t);

    auto m3 = core::mat4x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                            { 5.0f, 6.0f, 7.0f, 8.0f },
                            { 9.0f, 10.0f, 11.0f, 12.0f },
                            { 13.0f, 14.0f, 15.0f, 16.0f });
    auto m3t = core::mat4x4f({ 1.0f, 5.0f, 9.0f, 13.0f },
                             { 2.0f, 6.0f, 10.0f, 14.0f },
                             { 3.0f, 7.0f, 11.0f, 15.0f },
                             { 4.0f, 8.0f, 12.0f, 16.0f });
    Assert(core::mtranspose(m3) == m3t);
}

void mat_inverse() {
    {
        // 2x2
        auto m = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
        auto mi = core::mat2x2f({ -2.0f, 1.0f }, { 1.5f, -0.5f });
        auto res = core::minverse(m);
        Assert(res == mi);
    }
    {
        // 3x3
        auto m = core::mat3x3f({ 1, 2, 3 },
                               { 3, 2, 1 },
                               { 2, 1, 3 });
        auto mi = core::mat3x3f({ -0.416666687, 0.25f, 0.333333343f },
                                { 0.583333373f, 0.25f, -0.666666687f },
                                { 0.0833333358, -0.25f, 0.333333343f });
        auto res = core::minverse(m);
        Assert(res == mi);
    }
    {
        // 4x4
        auto m = core::mat4x4f({ 1, 4, 5, -1 },
                               { -2, 3, -1, 0 },
                               { 2, 1, 1, 0 },
                               { 3, -1, 2, 1 });
        auto mi = core::mat4x4f({ -0.100000001, -0.100000001, 0.600000024, -0.100000001 },
                                { 0, 0.25f, 0.25f, 0 },
                                { 0.200000003, -0.0500000007, -0.450000018, 0.200000003 },
                                { -0.100000001, 0.650000036, -0.650000036, 0.900000036 });
        auto res = core::minverse(m);
        Assert(res == mi);
    }
}

void run_mat_tests_suite() {
    RunTest(mat_equals);
    RunTest(mat2xN_constructors);
    RunTest(mat3xN_constructors);
    RunTest(mat4xN_constructors);
    RunTest(mat_add);
    RunTest(mat_sub);
    RunTest(mat_mul);
    RunTest(mat_determinant);
    RunTest(mat_identity);
    RunTest(mat_transpose);
    RunTest(mat_inverse);
}
