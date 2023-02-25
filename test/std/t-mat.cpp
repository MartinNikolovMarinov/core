void mat1xN_constructors() {
    {
        core::mat<1, 1, f32> m1(1.0f);
        Assert(m1[0].x() == 1.0f);
        Assert(m1.data[0].x() == 1.0f);
        Assert(m1.data[0][0] == 1.0f);
        Assert(m1.data[0].data[0] == 1.0f);

        core::mat<1, 1, f32> m2(core::vec<1, f32>(1.0f));
        Assert(m2[0].x() == 1.0f);
        Assert(m2.data[0].x() == 1.0f);
        Assert(m2.data[0][0] == 1.0f);
        Assert(m2.data[0].data[0] == 1.0f);
    }
    {
        core::mat<1, 2, f32> m(core::vec2f(1.0f, 2.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
    }
    {
        core::mat<1, 3, f32> m(core::vec3f(1.0f, 2.0f, 3.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[0].z() == 3.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[0].z() == 3.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[0][2] == 3.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[0].data[2] == 3.0f);
    }
    {
        core::mat<1, 4, f32> m(core::vec4f(1.0f, 2.0f, 3.0f, 4.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[0].z() == 3.0f);
        Assert(m[0].w() == 4.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[0].z() == 3.0f);
        Assert(m.data[0].w() == 4.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[0][2] == 3.0f);
        Assert(m.data[0][3] == 4.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[0].data[2] == 3.0f);
        Assert(m.data[0].data[3] == 4.0f);
    }
}

void mat2xN_constructors() {
    {
        core::mat<2, 1, f32> m(core::vec<1, f32>(1.0f), core::vec<1, f32>(2.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[1].x() == 2.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[1].x() == 2.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[1][0] == 2.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[1].data[0] == 2.0f);
    }

    {
        core::mat2x2f m1(1.0f, 2.0f, 3.0f, 4.0f);
        Assert(m1[0].x() == 1.0f);
        Assert(m1[0].y() == 2.0f);
        Assert(m1[1].x() == 3.0f);
        Assert(m1[1].y() == 4.0f);
        Assert(m1.data[0].x() == 1.0f);
        Assert(m1.data[0].y() == 2.0f);
        Assert(m1.data[1].x() == 3.0f);
        Assert(m1.data[1].y() == 4.0f);
        Assert(m1.data[0][0] == 1.0f);
        Assert(m1.data[0][1] == 2.0f);
        Assert(m1.data[1][0] == 3.0f);
        Assert(m1.data[1][1] == 4.0f);
        Assert(m1.data[0].data[0] == 1.0f);
        Assert(m1.data[0].data[1] == 2.0f);
        Assert(m1.data[1].data[0] == 3.0f);
        Assert(m1.data[1].data[1] == 4.0f);

        core::mat2x2f m2(core::vec2f(1.0f, 2.0f), core::vec2f(3.0f, 4.0f));
        Assert(m2[0].x() == 1.0f);
        Assert(m2[0].y() == 2.0f);
        Assert(m2[1].x() == 3.0f);
        Assert(m2[1].y() == 4.0f);
        Assert(m2.data[0].x() == 1.0f);
        Assert(m2.data[0].y() == 2.0f);
        Assert(m2.data[1].x() == 3.0f);
        Assert(m2.data[1].y() == 4.0f);
        Assert(m2.data[0][0] == 1.0f);
        Assert(m2.data[0][1] == 2.0f);
        Assert(m2.data[1][0] == 3.0f);
        Assert(m2.data[1][1] == 4.0f);
        Assert(m2.data[0].data[0] == 1.0f);
        Assert(m2.data[0].data[1] == 2.0f);
        Assert(m2.data[1].data[0] == 3.0f);
        Assert(m2.data[1].data[1] == 4.0f);
    }

    {
        core::mat2x3f m(core::vec3f(1.0f, 2.0f, 3.0f), core::vec3f(4.0f, 5.0f, 6.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[0].z() == 3.0f);
        Assert(m[1].x() == 4.0f);
        Assert(m[1].y() == 5.0f);
        Assert(m[1].z() == 6.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[0].z() == 3.0f);
        Assert(m.data[1].x() == 4.0f);
        Assert(m.data[1].y() == 5.0f);
        Assert(m.data[1].z() == 6.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[0][2] == 3.0f);
        Assert(m.data[1][0] == 4.0f);
        Assert(m.data[1][1] == 5.0f);
        Assert(m.data[1][2] == 6.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[0].data[2] == 3.0f);
        Assert(m.data[1].data[0] == 4.0f);
        Assert(m.data[1].data[1] == 5.0f);
        Assert(m.data[1].data[2] == 6.0f);
    }

    {
        core::mat2x4f m(core::vec4f(1.0f, 2.0f, 3.0f, 4.0f), core::vec4f(5.0f, 6.0f, 7.0f, 8.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[0].z() == 3.0f);
        Assert(m[0].w() == 4.0f);
        Assert(m[1].x() == 5.0f);
        Assert(m[1].y() == 6.0f);
        Assert(m[1].z() == 7.0f);
        Assert(m[1].w() == 8.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[0].z() == 3.0f);
        Assert(m.data[0].w() == 4.0f);
        Assert(m.data[1].x() == 5.0f);
        Assert(m.data[1].y() == 6.0f);
        Assert(m.data[1].z() == 7.0f);
        Assert(m.data[1].w() == 8.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[0][2] == 3.0f);
        Assert(m.data[0][3] == 4.0f);
        Assert(m.data[1][0] == 5.0f);
        Assert(m.data[1][1] == 6.0f);
        Assert(m.data[1][2] == 7.0f);
        Assert(m.data[1][3] == 8.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[0].data[2] == 3.0f);
        Assert(m.data[0].data[3] == 4.0f);
        Assert(m.data[1].data[0] == 5.0f);
        Assert(m.data[1].data[1] == 6.0f);
        Assert(m.data[1].data[2] == 7.0f);
        Assert(m.data[1].data[3] == 8.0f);
    }
}

void mat3xN_constructors() {
    {
        core::mat<3, 1, f32> m(core::vec<1, f32>(1.0f), core::vec<1, f32>(2.0f), core::vec<1, f32>(3.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[1].x() == 2.0f);
        Assert(m[2].x() == 3.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[1].x() == 2.0f);
        Assert(m.data[2].x() == 3.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[1][0] == 2.0f);
        Assert(m.data[2][0] == 3.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[1].data[0] == 2.0f);
        Assert(m.data[2].data[0] == 3.0f);
    }

    {
        core::mat3x2f m(core::vec2f(1.0f, 2.0f), core::vec2f(3.0f, 4.0f), core::vec2f(5.0f, 6.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[1].x() == 3.0f);
        Assert(m[1].y() == 4.0f);
        Assert(m[2].x() == 5.0f);
        Assert(m[2].y() == 6.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[1].x() == 3.0f);
        Assert(m.data[1].y() == 4.0f);
        Assert(m.data[2].x() == 5.0f);
        Assert(m.data[2].y() == 6.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[1][0] == 3.0f);
        Assert(m.data[1][1] == 4.0f);
        Assert(m.data[2][0] == 5.0f);
        Assert(m.data[2][1] == 6.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[1].data[0] == 3.0f);
        Assert(m.data[1].data[1] == 4.0f);
        Assert(m.data[2].data[0] == 5.0f);
        Assert(m.data[2].data[1] == 6.0f);
    }

    {
        core::mat3x3f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        Assert(m1[0].x() == 1.0f);
        Assert(m1[0].y() == 2.0f);
        Assert(m1[0].z() == 3.0f);
        Assert(m1[1].x() == 4.0f);
        Assert(m1[1].y() == 5.0f);
        Assert(m1[1].z() == 6.0f);
        Assert(m1[2].x() == 7.0f);
        Assert(m1[2].y() == 8.0f);
        Assert(m1[2].z() == 9.0f);
        Assert(m1.data[0].x() == 1.0f);
        Assert(m1.data[0].y() == 2.0f);
        Assert(m1.data[0].z() == 3.0f);
        Assert(m1.data[1].x() == 4.0f);
        Assert(m1.data[1].y() == 5.0f);
        Assert(m1.data[1].z() == 6.0f);
        Assert(m1.data[2].x() == 7.0f);
        Assert(m1.data[2].y() == 8.0f);
        Assert(m1.data[2].z() == 9.0f);
        Assert(m1.data[0][0] == 1.0f);
        Assert(m1.data[0][1] == 2.0f);
        Assert(m1.data[0][2] == 3.0f);
        Assert(m1.data[1][0] == 4.0f);
        Assert(m1.data[1][1] == 5.0f);
        Assert(m1.data[1][2] == 6.0f);
        Assert(m1.data[2][0] == 7.0f);
        Assert(m1.data[2][1] == 8.0f);
        Assert(m1.data[2][2] == 9.0f);
        Assert(m1.data[0].data[0] == 1.0f);
        Assert(m1.data[0].data[1] == 2.0f);
        Assert(m1.data[0].data[2] == 3.0f);
        Assert(m1.data[1].data[0] == 4.0f);
        Assert(m1.data[1].data[1] == 5.0f);
        Assert(m1.data[1].data[2] == 6.0f);
        Assert(m1.data[2].data[0] == 7.0f);
        Assert(m1.data[2].data[1] == 8.0f);
        Assert(m1.data[2].data[2] == 9.0f);

        core::mat3x3f m2(core::vec3f(1.0f, 2.0f, 3.0f), core::vec3f(4.0f, 5.0f, 6.0f), core::vec3f(7.0f, 8.0f, 9.0f));
        Assert(m2[0].x() == 1.0f);
        Assert(m2[0].y() == 2.0f);
        Assert(m2[0].z() == 3.0f);
        Assert(m2[1].x() == 4.0f);
        Assert(m2[1].y() == 5.0f);
        Assert(m2[1].z() == 6.0f);
        Assert(m2[2].x() == 7.0f);
        Assert(m2[2].y() == 8.0f);
        Assert(m2[2].z() == 9.0f);
        Assert(m2.data[0].x() == 1.0f);
        Assert(m2.data[0].y() == 2.0f);
        Assert(m2.data[0].z() == 3.0f);
        Assert(m2.data[1].x() == 4.0f);
        Assert(m2.data[1].y() == 5.0f);
        Assert(m2.data[1].z() == 6.0f);
        Assert(m2.data[2].x() == 7.0f);
        Assert(m2.data[2].y() == 8.0f);
        Assert(m2.data[2].z() == 9.0f);
        Assert(m2.data[0][0] == 1.0f);
        Assert(m2.data[0][1] == 2.0f);
        Assert(m2.data[0][2] == 3.0f);
        Assert(m2.data[1][0] == 4.0f);
        Assert(m2.data[1][1] == 5.0f);
        Assert(m2.data[1][2] == 6.0f);
        Assert(m2.data[2][0] == 7.0f);
        Assert(m2.data[2][1] == 8.0f);
        Assert(m2.data[2][2] == 9.0f);
        Assert(m2.data[0].data[0] == 1.0f);
        Assert(m2.data[0].data[1] == 2.0f);
        Assert(m2.data[0].data[2] == 3.0f);
        Assert(m2.data[1].data[0] == 4.0f);
        Assert(m2.data[1].data[1] == 5.0f);
        Assert(m2.data[1].data[2] == 6.0f);
        Assert(m2.data[2].data[0] == 7.0f);
        Assert(m2.data[2].data[1] == 8.0f);
        Assert(m2.data[2].data[2] == 9.0f);
    }

    {
        core::mat3x4f m(core::vec4f(1.0f, 2.0f, 3.0f, 4.0f), core::vec4f(5.0f, 6.0f, 7.0f, 8.0f), core::vec4f(9.0f, 10.0f, 11.0f, 12.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[0].z() == 3.0f);
        Assert(m[0].w() == 4.0f);
        Assert(m[1].x() == 5.0f);
        Assert(m[1].y() == 6.0f);
        Assert(m[1].z() == 7.0f);
        Assert(m[1].w() == 8.0f);
        Assert(m[2].x() == 9.0f);
        Assert(m[2].y() == 10.0f);
        Assert(m[2].z() == 11.0f);
        Assert(m[2].w() == 12.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[0].z() == 3.0f);
        Assert(m.data[0].w() == 4.0f);
        Assert(m.data[1].x() == 5.0f);
        Assert(m.data[1].y() == 6.0f);
        Assert(m.data[1].z() == 7.0f);
        Assert(m.data[1].w() == 8.0f);
        Assert(m.data[2].x() == 9.0f);
        Assert(m.data[2].y() == 10.0f);
        Assert(m.data[2].z() == 11.0f);
        Assert(m.data[2].w() == 12.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[0][2] == 3.0f);
        Assert(m.data[0][3] == 4.0f);
        Assert(m.data[1][0] == 5.0f);
        Assert(m.data[1][1] == 6.0f);
        Assert(m.data[1][2] == 7.0f);
        Assert(m.data[1][3] == 8.0f);
        Assert(m.data[2][0] == 9.0f);
        Assert(m.data[2][1] == 10.0f);
        Assert(m.data[2][2] == 11.0f);
        Assert(m.data[2][3] == 12.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[0].data[2] == 3.0f);
        Assert(m.data[0].data[3] == 4.0f);
        Assert(m.data[1].data[0] == 5.0f);
        Assert(m.data[1].data[1] == 6.0f);
        Assert(m.data[1].data[2] == 7.0f);
        Assert(m.data[1].data[3] == 8.0f);
        Assert(m.data[2].data[0] == 9.0f);
        Assert(m.data[2].data[1] == 10.0f);
        Assert(m.data[2].data[2] == 11.0f);
        Assert(m.data[2].data[3] == 12.0f);
    }
}

void mat4xN_constructors() {
    {
        core::mat<4, 1, f32> m(core::vec<1, f32>(1.0f), core::vec<1, f32>(2.0f), core::vec<1, f32>(3.0f), core::vec<1, f32>(4.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[1].x() == 2.0f);
        Assert(m[2].x() == 3.0f);
        Assert(m[3].x() == 4.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[1].x() == 2.0f);
        Assert(m.data[2].x() == 3.0f);
        Assert(m.data[3].x() == 4.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[1][0] == 2.0f);
        Assert(m.data[2][0] == 3.0f);
        Assert(m.data[3][0] == 4.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[1].data[0] == 2.0f);
        Assert(m.data[2].data[0] == 3.0f);
        Assert(m.data[3].data[0] == 4.0f);
    }
    {
        core::mat4x2f m(core::vec2f(1.0f, 2.0f), core::vec2f(3.0f, 4.0f), core::vec2f(5.0f, 6.0f), core::vec2f(7.0f, 8.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[1].x() == 3.0f);
        Assert(m[1].y() == 4.0f);
        Assert(m[2].x() == 5.0f);
        Assert(m[2].y() == 6.0f);
        Assert(m[3].x() == 7.0f);
        Assert(m[3].y() == 8.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[1].x() == 3.0f);
        Assert(m.data[1].y() == 4.0f);
        Assert(m.data[2].x() == 5.0f);
        Assert(m.data[2].y() == 6.0f);
        Assert(m.data[3].x() == 7.0f);
        Assert(m.data[3].y() == 8.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[1][0] == 3.0f);
        Assert(m.data[1][1] == 4.0f);
        Assert(m.data[2][0] == 5.0f);
        Assert(m.data[2][1] == 6.0f);
        Assert(m.data[3][0] == 7.0f);
        Assert(m.data[3][1] == 8.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[1].data[0] == 3.0f);
        Assert(m.data[1].data[1] == 4.0f);
        Assert(m.data[2].data[0] == 5.0f);
        Assert(m.data[2].data[1] == 6.0f);
        Assert(m.data[3].data[0] == 7.0f);
        Assert(m.data[3].data[1] == 8.0f);
    }
    {
        core::mat4x3f m(core::vec3f(1.0f, 2.0f, 3.0f), core::vec3f(4.0f, 5.0f, 6.0f), core::vec3f(7.0f, 8.0f, 9.0f), core::vec3f(10.0f, 11.0f, 12.0f));
        Assert(m[0].x() == 1.0f);
        Assert(m[0].y() == 2.0f);
        Assert(m[0].z() == 3.0f);
        Assert(m[1].x() == 4.0f);
        Assert(m[1].y() == 5.0f);
        Assert(m[1].z() == 6.0f);
        Assert(m[2].x() == 7.0f);
        Assert(m[2].y() == 8.0f);
        Assert(m[2].z() == 9.0f);
        Assert(m[3].x() == 10.0f);
        Assert(m[3].y() == 11.0f);
        Assert(m[3].z() == 12.0f);
        Assert(m.data[0].x() == 1.0f);
        Assert(m.data[0].y() == 2.0f);
        Assert(m.data[0].z() == 3.0f);
        Assert(m.data[1].x() == 4.0f);
        Assert(m.data[1].y() == 5.0f);
        Assert(m.data[1].z() == 6.0f);
        Assert(m.data[2].x() == 7.0f);
        Assert(m.data[2].y() == 8.0f);
        Assert(m.data[2].z() == 9.0f);
        Assert(m.data[3].x() == 10.0f);
        Assert(m.data[3].y() == 11.0f);
        Assert(m.data[3].z() == 12.0f);
        Assert(m.data[0][0] == 1.0f);
        Assert(m.data[0][1] == 2.0f);
        Assert(m.data[0][2] == 3.0f);
        Assert(m.data[1][0] == 4.0f);
        Assert(m.data[1][1] == 5.0f);
        Assert(m.data[1][2] == 6.0f);
        Assert(m.data[2][0] == 7.0f);
        Assert(m.data[2][1] == 8.0f);
        Assert(m.data[2][2] == 9.0f);
        Assert(m.data[3][0] == 10.0f);
        Assert(m.data[3][1] == 11.0f);
        Assert(m.data[3][2] == 12.0f);
        Assert(m.data[0].data[0] == 1.0f);
        Assert(m.data[0].data[1] == 2.0f);
        Assert(m.data[0].data[2] == 3.0f);
        Assert(m.data[1].data[0] == 4.0f);
        Assert(m.data[1].data[1] == 5.0f);
        Assert(m.data[1].data[2] == 6.0f);
        Assert(m.data[2].data[0] == 7.0f);
        Assert(m.data[2].data[1] == 8.0f);
        Assert(m.data[2].data[2] == 9.0f);
        Assert(m.data[3].data[0] == 10.0f);
        Assert(m.data[3].data[1] == 11.0f);
        Assert(m.data[3].data[2] == 12.0f);
    }

    {
        core::mat4x4f m1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);        Assert(m1.data[0].x() == 1.0f);
        Assert(m1[0].y() == 2.0f);
        Assert(m1[0].z() == 3.0f);
        Assert(m1[0].w() == 4.0f);
        Assert(m1[1].x() == 5.0f);
        Assert(m1[1].y() == 6.0f);
        Assert(m1[1].z() == 7.0f);
        Assert(m1[1].w() == 8.0f);
        Assert(m1[2].x() == 9.0f);
        Assert(m1[2].y() == 10.0f);
        Assert(m1[2].z() == 11.0f);
        Assert(m1[2].w() == 12.0f);
        Assert(m1[3].x() == 13.0f);
        Assert(m1[3].y() == 14.0f);
        Assert(m1[3].z() == 15.0f);
        Assert(m1[3].w() == 16.0f);
        Assert(m1.data[0].y() == 2.0f);
        Assert(m1.data[0].z() == 3.0f);
        Assert(m1.data[0].w() == 4.0f);
        Assert(m1.data[1].x() == 5.0f);
        Assert(m1.data[1].y() == 6.0f);
        Assert(m1.data[1].z() == 7.0f);
        Assert(m1.data[1].w() == 8.0f);
        Assert(m1.data[2].x() == 9.0f);
        Assert(m1.data[2].y() == 10.0f);
        Assert(m1.data[2].z() == 11.0f);
        Assert(m1.data[2].w() == 12.0f);
        Assert(m1.data[3].x() == 13.0f);
        Assert(m1.data[3].y() == 14.0f);
        Assert(m1.data[3].z() == 15.0f);
        Assert(m1.data[3].w() == 16.0f);
        Assert(m1.data[0][0] == 1.0f);
        Assert(m1.data[0][1] == 2.0f);
        Assert(m1.data[0][2] == 3.0f);
        Assert(m1.data[0][3] == 4.0f);
        Assert(m1.data[1][0] == 5.0f);
        Assert(m1.data[1][1] == 6.0f);
        Assert(m1.data[1][2] == 7.0f);
        Assert(m1.data[1][3] == 8.0f);
        Assert(m1.data[2][0] == 9.0f);
        Assert(m1.data[2][1] == 10.0f);
        Assert(m1.data[2][2] == 11.0f);
        Assert(m1.data[2][3] == 12.0f);
        Assert(m1.data[3][0] == 13.0f);
        Assert(m1.data[3][1] == 14.0f);
        Assert(m1.data[3][2] == 15.0f);
        Assert(m1.data[3][3] == 16.0f);
        Assert(m1.data[0].data[0] == 1.0f);
        Assert(m1.data[0].data[1] == 2.0f);
        Assert(m1.data[0].data[2] == 3.0f);
        Assert(m1.data[0].data[3] == 4.0f);
        Assert(m1.data[1].data[0] == 5.0f);
        Assert(m1.data[1].data[1] == 6.0f);
        Assert(m1.data[1].data[2] == 7.0f);
        Assert(m1.data[1].data[3] == 8.0f);
        Assert(m1.data[2].data[0] == 9.0f);
        Assert(m1.data[2].data[1] == 10.0f);
        Assert(m1.data[2].data[2] == 11.0f);
        Assert(m1.data[2].data[3] == 12.0f);
        Assert(m1.data[3].data[0] == 13.0f);
        Assert(m1.data[3].data[1] == 14.0f);
        Assert(m1.data[3].data[2] == 15.0f);
        Assert(m1.data[3].data[3] == 16.0f);

        core::mat4x4f m2(core::vec4f(1.0f, 2.0f, 3.0f, 4.0f), core::vec4f(5.0f, 6.0f, 7.0f, 8.0f), core::vec4f(9.0f, 10.0f, 11.0f, 12.0f), core::vec4f(13.0f, 14.0f, 15.0f, 16.0f));
        Assert(m1[0].y() == 2.0f);
        Assert(m1[0].z() == 3.0f);
        Assert(m1[0].w() == 4.0f);
        Assert(m1[1].x() == 5.0f);
        Assert(m1[1].y() == 6.0f);
        Assert(m1[1].z() == 7.0f);
        Assert(m1[1].w() == 8.0f);
        Assert(m1[2].x() == 9.0f);
        Assert(m1[2].y() == 10.0f);
        Assert(m1[2].z() == 11.0f);
        Assert(m1[2].w() == 12.0f);
        Assert(m1[3].x() == 13.0f);
        Assert(m1[3].y() == 14.0f);
        Assert(m1[3].z() == 15.0f);
        Assert(m1[3].w() == 16.0f);
        Assert(m2.data[0].x() == 1.0f);
        Assert(m2.data[0].y() == 2.0f);
        Assert(m2.data[0].z() == 3.0f);
        Assert(m2.data[0].w() == 4.0f);
        Assert(m2.data[1].x() == 5.0f);
        Assert(m2.data[1].y() == 6.0f);
        Assert(m2.data[1].z() == 7.0f);
        Assert(m2.data[1].w() == 8.0f);
        Assert(m2.data[2].x() == 9.0f);
        Assert(m2.data[2].y() == 10.0f);
        Assert(m2.data[2].z() == 11.0f);
        Assert(m2.data[2].w() == 12.0f);
        Assert(m2.data[3].x() == 13.0f);
        Assert(m2.data[3].y() == 14.0f);
        Assert(m2.data[3].z() == 15.0f);
        Assert(m2.data[3].w() == 16.0f);
        Assert(m2.data[0][0] == 1.0f);
        Assert(m2.data[0][1] == 2.0f);
        Assert(m2.data[0][2] == 3.0f);
        Assert(m2.data[0][3] == 4.0f);
        Assert(m2.data[1][0] == 5.0f);
        Assert(m2.data[1][1] == 6.0f);
        Assert(m2.data[1][2] == 7.0f);
        Assert(m2.data[1][3] == 8.0f);
        Assert(m2.data[2][0] == 9.0f);
        Assert(m2.data[2][1] == 10.0f);
        Assert(m2.data[2][2] == 11.0f);
        Assert(m2.data[2][3] == 12.0f);
        Assert(m2.data[3][0] == 13.0f);
        Assert(m2.data[3][1] == 14.0f);
        Assert(m2.data[3][2] == 15.0f);
        Assert(m2.data[3][3] == 16.0f);
        Assert(m2.data[0].data[0] == 1.0f);
        Assert(m2.data[0].data[1] == 2.0f);
        Assert(m2.data[0].data[2] == 3.0f);
        Assert(m2.data[0].data[3] == 4.0f);
        Assert(m2.data[1].data[0] == 5.0f);
        Assert(m2.data[1].data[1] == 6.0f);
        Assert(m2.data[1].data[2] == 7.0f);
        Assert(m2.data[1].data[3] == 8.0f);
        Assert(m2.data[2].data[0] == 9.0f);
        Assert(m2.data[2].data[1] == 10.0f);
        Assert(m2.data[2].data[2] == 11.0f);
        Assert(m2.data[2].data[3] == 12.0f);
        Assert(m2.data[3].data[0] == 13.0f);
        Assert(m2.data[3].data[1] == 14.0f);
        Assert(m2.data[3].data[2] == 15.0f);
        Assert(m2.data[3].data[3] == 16.0f);
    }
}

void mat_equals() {
    core::mat2x2f m1(1.0f, 2.0f, 3.0f, 4.0f);
    core::mat2x2f m2(1.0f, 2.0f, 3.0f, 4.0f);
    core::mat2x2f m3(9.0f, 9.0f, 9.0f, 9.0f);
    Assert(core::mequals(m1, m2));
    Assert(core::mequals(m2, m1));
    Assert(m1.equals(m2));
    Assert(m2.equals(m1));
    Assert(m1 == m2);
    Assert(m2 == m1);
    Assert(m1 != m3);
    Assert(m3 != m1);
    Assert(m2 != m3);
    Assert(m3 != m2);

    // test safe msafeequals
    core::mat2x2f m4(1.0f, 2.0f, 3.0f, 4.0f);
    core::mat2x2f m5(1.000001f, 2.00002f, 3.0003f, 4.000999f);
    Assert(core::msafeequals(m4, m5, 0.001f));
    Assert(core::msafeequals(m5, m4, 0.0001f) == false);
    Assert(m4.equals(m5, 0.001f));
    Assert(m5.equals(m4, 0.0001f) == false);
}

void mat_add() {
    {
        core::mat2x2f m1(1.0f, 2.0f, 3.0f, 4.0f);
        core::mat2x2f m2(1.0f, 2.0f, 3.0f, 4.0f);
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

        core::mat3x3f m3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        core::mat3x3f m4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
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
        core::mat2x2f m1(1.0f, 2.0f, 3.0f, 4.0f);
        core::mat2x2f m2(1.0f, 2.0f, 3.0f, 4.0f);
        Assert(m1 + m2 == core::mat2x2f(2.0f, 4.0f, 6.0f, 8.0f));
        Assert(m2 + m1 == core::mat2x2f(2.0f, 4.0f, 6.0f, 8.0f));
        core::mat2x2f c(1.0f, 1.0f, 1.0f, 1.0f);
        Assert(m1 + m2 - c == core::mat2x2f(1.0f, 3.0f, 5.0f, 7.0f));

        core::mat3x3f m3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        core::mat3x3f m4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        core::mat3x3f c2(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        Assert(m3 + m4 == core::mat3x3f(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f));
        Assert(m4 + m3 == core::mat3x3f(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f));
        Assert(-c2 + m3 + m4 == core::mat3x3f(1.0f, 3.0f, 5.0f, 7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f));
    }
}

void mat_sub() {
    {
        core::mat2x2f m1(2.0f, 3.0f, 4.0f, 5.0f);
        core::mat2x2f m2(1.0f, 2.0f, 3.0f, 4.0f);
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

        core::mat3x3f m3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f);
        core::mat3x3f m4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
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
        core::mat2x2f m1(2.0f, 3.0f, 4.0f, 5.0f);
        core::mat2x2f m2(1.0f, 2.0f, 3.0f, 4.0f);
        Assert(m1 - m2 == core::mat2x2f(1.0f, 1.0f, 1.0f, 1.0f));
        Assert(m2 - m1 == core::mat2x2f(-1.0f, -1.0f, -1.0f, -1.0f));
        core::mat2x2f c1(1.0f, 1.0f, 1.0f, 1.0f);
        Assert(m1 - m2 - c1 == core::mat2x2f(0.0f, 0.0f, 0.0f, 0.0f));

        core::mat3x3f m3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f);
        core::mat3x3f m4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
        Assert(m3 - m4 == core::mat3x3f(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
        Assert(m4 - m3 == core::mat3x3f(-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f));
    }
}

void run_mat_tests_suite() {
    RunTest(mat1xN_constructors);
    RunTest(mat2xN_constructors);
    RunTest(mat3xN_constructors);
    RunTest(mat4xN_constructors);
    RunTest(mat_equals);
    RunTest(mat_add);
    RunTest(mat_sub);
}
