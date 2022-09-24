using namespace core::math;

void VectorEqualsTest() {
    {
        vec2<i32> a { 1, 2 };
        vec2<i32> b { 1, 2 };
        Assert(a.equals(b));
        vec2<i32> c { 2, 1 };
        Assert(!a.equals(c));
    }
    {
        vec3<i32> a { 1, 2, 3 };
        vec3<i32> b { 1, 2, 3 };
        Assert(a.equals(b));
        vec3<i32> c { 2, 1, 3 };
        Assert(!a.equals(c));
    }
    {
        vec4<i32> a { 1, 2, 3, 4 };
        vec4<i32> b { 1, 2, 3, 4 };
        Assert(a.equals(b));
        vec4<i32> c { 2, 1, 3, 4 };
        Assert(!a.equals(c));
    }
    {
        vec2<f32> a { 1.0f, 2.0f };
        vec2<f32> b { 1.0f, 2.1f };
        Assert(!a.equals(b));
        Assert(!a.equals(b, 0.09f));
        Assert(!a.equals(b, 0.09999f));
        Assert(a.equals(b, 0.10001f));
    }
}

void VectorAddSubMulDivTest() {
    {
        vec2<i32> a { 1, 2 };
        vec2<i32> b { 3, 4 };
        a.add(b); Assert(a.equals(v(4, 6)));
        a.sub(b); Assert(a.equals(v(1, 2)));
        a.mul(b); Assert(a.equals(v(3, 8)));
        a.div(b); Assert(a.equals(v(1, 2)));
    }
    {
        vec3<i32> a { 1, 2, 3 };
        vec3<i32> b { 3, 4, 5 };
        a.add(b); Assert(a.equals(v(4, 6, 8)));
        a.sub(b); Assert(a.equals(v(1, 2, 3)));
        a.mul(b); Assert(a.equals(v(3, 8, 15)));
        a.div(b); Assert(a.equals(v(1, 2, 3)));
    }
    {
        auto a = v(1, 2, 3, 4);
        auto b = v(3, 4, 5, 6);
        a.add(b); Assert(a.equals(v(4, 6, 8, 10)));
        a.sub(b); Assert(a.equals(v(1, 2, 3, 4)));
        a.mul(b); Assert(a.equals(v(3, 8, 15, 24)));
        a.div(b); Assert(a.equals(v(1, 2, 3, 4)));
    }
    {
        vec2<f32> a = v(1.0f, 2.0f);
        vec2<i32> b = v(0, 1);
        a.add(b); Assert(a.equals(v(1.0f, 3.0f)));
        b.add(a); Assert(b.equals(v(1, 4)));
        a.sub(b); Assert(a.equals(v(0.0f, -1.0f)));
        b.sub(a); Assert(b.equals(v(1, 5)));
        a.mul(b); Assert(a.equals(v(0.0f, -5.0f)));
        b.mul(a); Assert(b.equals(v(0, -25)));
        // These will fail due to division by zero:
        // a.div(b); Assert(a.equals(v(0.0f, 0.2f)));
        // b.div(a); Assert(b.equals(v(0, -5)));
        a++;
        b++;
        a.div(b); Assert(a.equals(v(1.0f, 0.166666667f), 0.0000001f));
        a.inc();
        b.div(a); Assert(b.equals(v(0, -24)));
    }
    {
        vec2<f32> a = v(1.0f, 2.0f);
        vec2<f32> b = -a;
        Assert(b.equals(v(-1.0f, -2.0f)));
        Assert(b.equals(-a));
        Assert(a.equals(-b));
        b = -b;
        Assert(b.equals(a));
    }
}

void VectorLengthTest() {
    constexpr f32 epsillon = 0.000001f;
    Assert(std::abs(v(1, 2).length() - 2.23606797749979f) < epsillon);
    Assert(std::abs(v(1, 2, 3).length() - 3.7416573867739413f) < epsillon);
    Assert(std::abs(v(1, 2, 3, 4).length() - 5.477225575051661f) < epsillon);
    Assert(std::abs(v(1.0f, 2.0f).length() - 2.23606797749979f) < epsillon);
}

void VectorDotProductTest() {
    Assert(v(1, 2).dot(v(3, 4)) == 11.0f);
    Assert(v(1, 2, 3).dot(v(3, 4, 5)) == 26.0f);
    Assert(v(1, 2, 3, 4).dot(v(3, 4, 5, 6)) == 50.0f);
    Assert(v(1.0f, 2.0f).dot(v(3.0f, 4.0f)) == 11.0f);

    // Properties of dot product:
    {
        // A ⋅ B = B ⋅ A
        auto a = v(1, 2);
        auto b = v(3, 4);
        Assert(a.dot(b) == b.dot(a));
    }
    {
        // (x * A) ⋅ B = x * (A ⋅ B)
        auto a = v(1, 2);
        auto b = v(3, 4);
        i32 x = 2;
        Assert((x * a).dot(b) == a.dot(b) * x);
    }
    {
        // A ⋅ (B + C) = A ⋅ B + A ⋅ C
        auto a = v(1, 2);
        auto b = v(3, 4);
        auto c = v(5, 6);
        Assert(a.dot(b + c) == a.dot(b) + a.dot(c));
    }
    {
        // A ⋅ A = len(A)^2
        auto a = v(1, 2);
        constexpr f32 epsillon = 0.000001f;
        Assert(std::abs(a.dot(a) - a.length() * a.length()) < epsillon);
    }
    {
        // A ⋅ B ≤ len(A) * len(B)
        auto a = v(1, 2);
        auto b = v(1, 2);
        Assert(a.dot(b) <= a.length() * b.length());
    }
}

void VectorCrossProductTest() {
    Assert(v(1, 2, 3).cross(v(3, 4, 5)).equals(v(-2, 4, -2)));
    Assert(v(1.0f, 2.0f, 3.0f).cross(v(3.0f, 4.0f, 5.0f)).equals(v(-2.0f, 4.0f, -2.0f)));

    // Properties of the cross product:
    {
        // A × B = − (B × A)
        auto a = v(1, 2, 3);
        auto b = v(3, 4, 5);
        Assert(a.cross(b).equals(-b.cross(a)));
    }
    {
        // (x * B) × A = x (B × A)
        auto a = v(1, 2, 3);
        auto b = v(3, 4, 5);
        i32 x = 2;
        Assert((x * b).cross(a).equals(x * b.cross(a)));
    }
    {
        // B × (A + C) = B × A + B × C
        auto a = v(1, 2, 3);
        auto b = v(3, 4, 5);
        auto c = v(5, 6, 7);
        Assert(b.cross(a + c).equals(b.cross(a) + b.cross(c)));
    }
    {
        // B × B = (0,0,0)
        auto b = v(3, 4, 5);
        Assert(b.cross(b).equals(b.zero()));
    }
    {
        // (B × A) ⋅ C = (C × B) ⋅ A = (A × C) ⋅ B
        auto a = v(1, 2, 3);
        auto b = v(3, 4, 5);
        auto c = v(5, 6, 7);
        Assert(b.cross(a).dot(c) == c.cross(b).dot(a));
        Assert(c.cross(b).dot(a) == a.cross(c).dot(b));
    }
}

void VectorTestSuite() {
    RunTest(VectorEqualsTest);
    RunTest(VectorAddSubMulDivTest);
    RunTest(VectorLengthTest);
    RunTest(VectorDotProductTest);
    RunTest(VectorCrossProductTest);
}
