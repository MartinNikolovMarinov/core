void mat2xN_constructors() {}

void mat3xN_constructors() {}

void mat4xN_constructors() {}

void mat_equals() {
    auto m1 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
    auto m2 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
    auto m3 = core::mat2x2f(9.0f, 9.0f, 9.0f, 9.0f);
    Assert(core::mequals(m1, m2));
    Assert(core::mequals(m2, m1));
    Assert(m1 == m2);
    Assert(m2 == m1);
    Assert(m1 != m3);
    Assert(m3 != m1);
    Assert(m2 != m3);
    Assert(m3 != m2);

    // test safe msafeequals
    auto m4 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
    auto m5 = core::mat2x2f(1.000001f, 2.00002f, 3.0003f, 4.000999f);
    Assert(core::msafeequals(m4, m5, 0.001f));
    Assert(core::msafeequals(m5, m4, 0.0001f) == false);

    // uneven matrices
    auto m6 = core::mat2x3i(core::v<i32>(1, 2, 3), core::v<i32>(5, 6, 7));
    auto m7 = core::mat2x3i(core::v<i32>(1, 2, 3), core::v<i32>(5, 6, 7));
    Assert(core::mequals(m6, m7));
    Assert(core::mequals(m7, m6));
}

void mat_add() {
    {
        auto m1 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        auto m2 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        m1.add(m2);
        Assert(m1 == core::mat2x2f(2.0f, 4.0f, 6.0f, 8.0f));
        m1 += m2;
        Assert(m1 == core::mat2x2f(3.0f, 6.0f, 9.0f, 12.0f));
        m1++;
        Assert(m1 == core::mat2x2f(4.0f, 7.0f, 10.0f, 13.0f));
        ++m1;
        Assert(m1 == core::mat2x2f(5.0f, 8.0f, 11.0f, 14.0f));
        m1 += 2.0f;
        Assert(m1 == core::mat2x2f(7.0f, 10.0f, 13.0f, 16.0f));

        auto m3 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        auto m4 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        m3.add(m4);
        Assert(m3 == core::mat3x3f(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f));
        m3 += m4;
        Assert(m3 == core::mat3x3f(3.0f, 6.0f, 9.0f, 12.0f, 15.0f, 18.0f, 21.0f, 24.0f, 27.0f));
        m3++;
        Assert(m3 == core::mat3x3f(4.0f, 7.0f, 10.0f, 13.0f, 16.0f, 19.0f, 22.0f, 25.0f, 28.0f));
        ++m3;
        Assert(m3 == core::mat3x3f(5.0f, 8.0f, 11.0f, 14.0f, 17.0f, 20.0f, 23.0f, 26.0f, 29.0f));
        m3 += 2.0f;
        Assert(m3 == core::mat3x3f(7.0f, 10.0f, 13.0f, 16.0f, 19.0f, 22.0f, 25.0f, 28.0f, 31.0f));
    }
    {
        auto m1 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        auto m2 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        Assert(m1 + m2 == core::mat2x2f(2.0f, 4.0f, 6.0f, 8.0f));
        Assert(m2 + m1 == core::mat2x2f(2.0f, 4.0f, 6.0f, 8.0f));
        auto c = core::mat2x2f(1.0f, 1.0f, 1.0f, 1.0f);
        Assert(m1 + m2 - c == core::mat2x2f(1.0f, 3.0f, 5.0f, 7.0f));

        auto m3 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        auto m4 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        auto c2 = core::mat3x3f(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        Assert(m3 + m4 == core::mat3x3f(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f));
        Assert(m4 + m3 == core::mat3x3f(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f));
        Assert(-c2 + m3 + m4 == core::mat3x3f(1.0f, 3.0f, 5.0f, 7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f));
    }
}

void mat_sub() {
    {
        auto m1 = core::mat2x2f(2.0f, 3.0f, 4.0f, 5.0f);
        auto m2 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        m1.sub(m2);
        Assert(m1 == core::mat2x2f(1.0f, 1.0f, 1.0f, 1.0f));
        m1 -= m2;
        Assert(m1 == core::mat2x2f(0.0f, -1.0f, -2.0f, -3.0f));
        m1--;
        Assert(m1 == core::mat2x2f(-1.0f, -2.0f, -3.0f, -4.0f));
        --m1;
        Assert(m1 == core::mat2x2f(-2.0f, -3.0f, -4.0f, -5.0f));
        m1 -= 2.0f;
        Assert(m1 == core::mat2x2f(-4.0f, -5.0f, -6.0f, -7.0f));

        auto m3 = core::mat3x3f(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f);
        auto m4 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        m3.sub(m4);
        Assert(m3 == core::mat3x3f(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        m3 -= m4;
        Assert(m3 == core::mat3x3f(0.0f, -1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f));
        m3--;
        Assert(m3 == core::mat3x3f(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f));
        --m3;
        Assert(m3 == core::mat3x3f(-2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f));
        m3 -= 2.0f;
        Assert(m3 == core::mat3x3f(-4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f));
    }
    {
        auto m1 = core::mat2x2f(2.0f, 3.0f, 4.0f, 5.0f);
        auto m2 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        Assert(m1 - m2 == core::mat2x2f(1.0f, 1.0f, 1.0f, 1.0f));
        Assert(m2 - m1 == core::mat2x2f(-1.0f, -1.0f, -1.0f, -1.0f));
        auto c1 = core::mat2x2f(1.0f, 1.0f, 1.0f, 1.0f);
        Assert(m1 - m2 - c1 == core::mat2x2f(0.0f, 0.0f, 0.0f, 0.0f));

        auto m3 = core::mat3x3f(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f);
        auto m4 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        Assert(m3 - m4 == core::mat3x3f(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        Assert(m4 - m3 == core::mat3x3f(-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f));
    }
    {
        auto m1 = core::mat2x2f(2.0f, 3.0f, 4.0f, 5.0f);
        auto m2 = core::m2x2i(1, 2, 3, 4);
        m1.sub(m2);
        Assert(m1 == core::mat2x2f(1.0f, 1.0f, 1.0f, 1.0f));
    }
}

void mat_mul() {
    // Matrix multiplication with scalars:
    {
        auto m1 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        m1.mul(2.0f);
        Assert(m1 == core::mat2x2f(2.0f, 4.0f, 6.0f, 8.0f));
        m1 *= 2.0f;
        Assert(m1 == core::mat2x2f(4.0f, 8.0f, 12.0f, 16.0f));
        m1 = m1 * 2.0f;
        Assert(m1 == core::mat2x2f(8.0f, 16.0f, 24.0f, 32.0f));
        m1 = 2.0f * m1;
        Assert(m1 == core::mat2x2f(16.0f, 32.0f, 48.0f, 64.0f));

        auto m3 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        m3.mul(2.0f);
        Assert(m3 == core::mat3x3f(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f));
        m3 *= 2.0f;
        Assert(m3 == core::mat3x3f(4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f, 28.0f, 32.0f, 36.0f));
        m3 = m3 * 2.0f;
        Assert(m3 == core::mat3x3f(8.0f, 16.0f, 24.0f, 32.0f, 40.0f, 48.0f, 56.0f, 64.0f, 72.0f));
        m3 = 2.0f * m3;
        Assert(m3 == core::mat3x3f(16.0f, 32.0f, 48.0f, 64.0f, 80.0f, 96.0f, 112.0f, 128.0f, 144.0f));

        auto m4 = core::m4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
        m4.mul(2.0f);
        Assert(m4 == core::m4x4(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f, 32.0f));
        m4 *= 2.0f;
        Assert(m4 == core::m4x4(4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f, 28.0f, 32.0f, 36.0f, 40.0f, 44.0f, 48.0f, 52.0f, 56.0f, 60.0f, 64.0f));
        m4 = m4 * 2.0f;
        Assert(m4 == core::m4x4(8.0f, 16.0f, 24.0f, 32.0f, 40.0f, 48.0f, 56.0f, 64.0f, 72.0f, 80.0f, 88.0f, 96.0f, 104.0f, 112.0f, 120.0f, 128.0f));
        m4 = 2.0f * m4;
        Assert(m4 == core::m4x4(16.0f, 32.0f, 48.0f, 64.0f, 80.0f, 96.0f, 112.0f, 128.0f, 144.0f, 160.0f, 176.0f, 192.0f, 208.0f, 224.0f, 240.0f, 256.0f));
    }
    // Matrix by matrix multiplication:
    {
        auto m1 = core::mat2x2f(1.0f, 2.0f, 3.0f, 4.0f);
        auto m2 = core::mat2x2f(5.0f, 6.0f, 7.0f, 8.0f);
        Assert(m1 * m2 == core::mat2x2f(19.0f, 22.0f, 43.0f, 50.0f));
        Assert(m2 * m1 == core::mat2x2f(23.0f, 34.0f, 31.0f, 46.0f));

        auto m3 = core::mat3x3f(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        auto m4 = core::mat3x3f(10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f);
        Assert(m3 * m4 == core::mat3x3f(84.0f, 90.0f, 96.0f, 201.0f, 216.0f, 231.0f, 318.0f, 342.0f, 366.0f));
        Assert(m4 * m3 == core::mat3x3f(138.0f, 171.0f, 204.0f, 174.0f, 216.0f, 258.0f, 210.0f, 261.0f, 312.0f));

        auto m5 = core::m4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
        auto m6 = core::m4x4(17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f, 30.0f, 31.0f, 32.0f);
        Assert(m5 * m6 == core::m4x4(250.0f, 260.0f, 270.0f, 280.0f, 618.0f, 644.0f, 670.0f, 696.0f, 986.0f, 1028.0f, 1070.0f, 1112.0f, 1354.0f, 1412.0f, 1470.0f, 1528.0f));
        Assert(m6 * m5 == core::m4x4(538.0f, 612.0f, 686.0f, 760.0f, 650.0f, 740.0f, 830.0f, 920.0f, 762.0f, 868.0f, 974.0f, 1080.0f, 874.0f, 996.0f, 1118.0f, 1240.0f));

        auto m7 = core::m2x3f({ 1.0f, 2.0f, 3.0f }, { 4.0f, 5.0f, 6.0f });
        auto m8 = core::m3x2f({ 7.0f, 8.0f }, { 9.0f, 10.0f }, { 11.0f, 12.0f });
        // m7 * m8 is ambiguous, so use a function call instead:
        Assert(core::mmul(m7, m8) == core::mat2x2f({ 58.0f, 64.0f }, { 139.0f, 154.0f }));
        Assert(core::mmul(m8, m7) == core::m3x3f({ 39.0f, 54.0f, 69.0f }, { 49.0f, 68.0f, 87.0f }, { 59.0f, 82.0f, 105.0f }));

        auto m9 = core::m3x4f({ 1.0f, 2.0f, 3.0f, 4.0f }, { 5.0f, 6.0f, 7.0f, 8.0f }, { 9.0f, 10.0f, 11.0f, 12.0f });
        auto m10 = core::m4x3f({ 13.0f, 14.0f, 15.0f }, { 16.0f, 17.0f, 18.0f }, { 19.0f, 20.0f, 21.0f }, { 22.0f, 23.0f, 24.0f });
        Assert(core::mmul(m9, m10) == core::m3x3f({ 190.0f, 200.0f, 210.0f }, { 470.0f, 496.0f, 522.0f }, { 750.0f, 792.0f, 834.0f }));
        Assert(core::mmul(m10, m9) == core::m4x4f({ 218.0f, 260.0f, 302.0f, 344.0f }, { 263.0f, 314.0f, 365.0f, 416.0f }, { 308.0f, 368.0f, 428.0f, 488.0f }, { 353.0f, 422.0f, 491.0f, 560.0f }));

        auto m11 = core::m4x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f }, { 5.0f, 6.0f }, { 7.0f, 8.0f });
        auto m12 = core::mat2x2f({ 9.0f, 10.0f }, { 11.0f, 12.0f });
        Assert(core::mmul(m11, m12) == core::m4x2f({ 31.0f, 34.0f }, { 71.0f, 78.0f }, { 111.0f, 122.0f }, { 151.0f, 166.0f }));
        // m12 * m11 is not allowed.
    }
    // Matrix by vector multiplication:
    {
        auto m1 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
        auto v1 = core::v(5.0f, 6.0f);
        Assert(m1 * v1 == core::v(17.0f, 39.0f));

        auto m2 = core::m3x3f({ 1.0f, 2.0f, 3.0f }, { 4.0f, 5.0f, 6.0f }, { 7.0f, 8.0f, 9.0f });
        auto v2 = core::v(10.0f, 11.0f, 12.0f);
        Assert(m2 * v2 == core::v(68.0f, 167.0f, 266.0f));

        auto m3 = core::m4x4f({ 1.0f, 2.0f, 3.0f, 4.0f }, { 5.0f, 6.0f, 7.0f, 8.0f }, { 9.0f, 10.0f, 11.0f, 12.0f }, { 13.0f, 14.0f, 15.0f, 16.0f });
        auto v3 = core::v(17.0f, 18.0f, 19.0f, 20.0f);
        Assert(m3 * v3 == core::v(190.0f, 486.0f, 782.0f, 1078.0f));

        auto m4 = core::m3x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f }, { 5.0f, 6.0f });
        auto v4 = core::v(7.0f, 8.0f);
        Assert(m4 * v4 == core::vf(23.0f, 53.0f, 83.0f));

        auto m5 = core::m1x2f({ 1.0f, 2.0f });
        auto v5 = core::v(3.0f, 4.0f);
        Assert(m5 * v5 == core::v(11.0f));

        auto m6 = core::m1x3f({ 1.0f, 2.0f, 3.0f });
        auto v6 = core::v(4.0f, 5.0f, 6.0f);
        Assert(m6 * v6 == core::v(32.0f));

        auto m7 = core::m1x4f({ 1.0f, 2.0f, 3.0f, 4.0f });
        auto v7 = core::v(5.0f, 6.0f, 7.0f, 8.0f);
        Assert(m7 * v7 == core::v(70.0f));

        auto m8 = core::m2x1f(1.0f, 2.0f);
        auto v8 = core::v(3.0f);
        Assert(m8 * v8 == core::v(3.0f, 6.0f));
    }
}

void mat_determinant() {
    auto m1 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    Assert(core::mdet(m1) == -2.0f);

    auto m2 = core::m2x2i({ 1, 2 }, { 3, 4 });
    Assert(core::mdet(m2) == -2);

    auto m3 = core::m3x3f({ 1.0f, 1.0f, 1.0f },
                              { 0.0f, 1.0f, 0.0f },
                              { 1.0f, 0.0f, 1.0f });
    Assert(core::mdet(m3) == 0.0f);

    auto m4 = core::m3x3i({ 2, 8, 1 }, { 0, 1, 3 }, { 4, 5, 1 });
    Assert(m4.det() == 64);

    auto m5 = core::m3x3f(0.0f);
    Assert(core::mdet(m5) == 0.0f);

    auto m6 = core::m4x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                              { 5.0f, 56.0f, 7.0f, 8.0f },
                              { 9.0f, 10.0f, 6.0f, 12.0f },
                              { 13.0f, 14.0f, 15.0f, 16.0f });
    Assert(m6.det() == 9000.0f);

    core::mat<5, 5, f32> m7;
    m7[0] = core::v(3.0f, 12.0f, 3.0f, 4.0f, 5.0f);
    m7[1] = core::v(2.0f, 34.0f, 12.0f, 1.0f, 3.0f);
    m7[2] = core::v(4.0f, 2.0f, 1.0f, 4.0f, 4.0f);
    m7[3] = core::v(5.0f, 6.0f, 2.0f, 3.0f, 5.0f);
    m7[4] = core::v(9.0f, 9.0f, 8.0f, 6.0f, 1.0f);
    Assert(core::nearly_eq(core::mdet(m7), 1494.99988f, 0.00001f));
}

void mat_identity() {
    auto m1 = core::mat2x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f });
    Assert(m1.identity() == core::mat2x2f({ 1.0f, 0.0f }, { 0.0f, 1.0f }));
    Assert(m1.identity() * m1 == m1);
    Assert(m1 * m1.identity() == m1);

    auto m2 = core::m3x3f({ 1.0f, 1.0f, 1.0f },
                              { 0.0f, 1.0f, 0.0f },
                              { 1.0f, 0.0f, 1.0f });
    Assert(m2.identity() == core::m3x3f({ 1.0f, 0.0f, 0.0f },
                                            { 0.0f, 1.0f, 0.0f },
                                            { 0.0f, 0.0f, 1.0f }));
    Assert(m2.identity() * m2 == m2);
    Assert(m2 * m2.identity() == m2);

    auto m3 = core::m4x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                              { 5.0f, 56.0f, 7.0f, 8.0f },
                              { 9.0f, 10.0f, 6.0f, 12.0f },
                              { 13.0f, 14.0f, 15.0f, 16.0f });
    Assert(m3.identity() == core::m4x4f({ 1.0f, 0.0f, 0.0f, 0.0f },
                                            { 0.0f, 1.0f, 0.0f, 0.0f },
                                            { 0.0f, 0.0f, 1.0f, 0.0f },
                                            { 0.0f, 0.0f, 0.0f, 1.0f }));
    Assert(m3.identity() * m3 == m3);
    Assert(m3 * m3.identity() == m3);

    // Identity for non-square matrices is allowed but there is a difference between left and right identity.
    // So the api is a bit more complex:
    auto m4 = core::m2x3f({ 1.0f, 2.0f, 3.0f }, { 4.0f, 5.0f, 6.0f });
    Assert(core::mmul(m4, core::midentity<m4.dimensionsCols(), f32>()) == m4);
    Assert(core::mmul(core::midentity<m4.dimensionsRows(), f32>(), m4) == m4);

    auto m5 = core::m3x2f({ 1.0f, 2.0f }, { 3.0f, 4.0f }, { 5.0f, 6.0f });
    Assert(core::mmul(m5, core::midentity<m5.dimensionsCols(), f32>()) == m5);
    Assert(core::mmul(core::midentity<m5.dimensionsRows(), f32>(), m5) == m5);

    auto m6 = core::m3x4f({ 1.0f, 2.0f, 3.0f, 4.0f },
                              { 5.0f, 6.0f, 7.0f, 8.0f },
                              { 9.0f, 10.0f, 11.0f, 12.0f });
    Assert(core::mmul(m6, core::midentity<m6.dimensionsCols(), f32>()) == m6);
    Assert(core::mmul(core::midentity<m6.dimensionsRows(), f32>(), m6) == m6);

    auto m7 = core::m4x3f({ 1.0f, 2.0f, 3.0f },
                              { 4.0f, 5.0f, 6.0f },
                              { 7.0f, 8.0f, 9.0f },
                              { 10.0f, 11.0f, 12.0f });
    Assert(core::mmul(m7, core::midentity<m7.dimensionsCols(), f32>()) == m7);
    Assert(core::mmul(core::midentity<m7.dimensionsRows(), f32>(), m7) == m7);

    auto m8 = core::m4x1f(1.0f, 2.0f, 3.0f, 4.0f);
    Assert(core::mmul(m8, core::midentity<m8.dimensionsCols(), f32>()) == m8);
    Assert(core::mmul(core::midentity<m8.dimensionsRows(), f32>(), m8) == m8);
}

void run_mat_tests_suite() {
    RunTest(mat2xN_constructors);
    RunTest(mat3xN_constructors);
    RunTest(mat4xN_constructors);
    RunTest(mat_equals);
    RunTest(mat_add);
    RunTest(mat_sub);
    RunTest(mat_mul);
    RunTest(mat_determinant);
    RunTest(mat_identity);
}
