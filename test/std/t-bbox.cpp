constexpr i32 bbox2D_center_test() {
    constexpr f32 epsilon = 0.0001f;

    struct TestCase {
        core::bbox_2d input;
        core::vec2f expected;
    };

    constexpr TestCase cases[] = {
        { core::bbox_2d(core::v(0.0f, 0.0f),   core::v(10.0f, 10.0f)), core::v(5.0f, 5.0f) },
        { core::bbox_2d(core::v(-5.0f, -5.0f), core::v(5.0f, 5.0f)),   core::v(0.0f, 0.0f) },
        { core::bbox_2d(core::v(-5.0f, 0.0f),  core::v(5.0f, 10.0f)),  core::v(0.0f, 5.0f) },
        { core::bbox_2d(core::v(0.0f, -5.0f),  core::v(10.0f, 5.0f)),  core::v(5.0f, 0.0f) },

        // Flip min and max
        { core::bbox_2d(core::v(10.0f, 10.0f), core::v(0.0f, 0.0f)),   core::v(5.0f, 5.0f) },
        { core::bbox_2d(core::v(5.0f, 5.0f),   core::v(-5.0f, -5.0f)), core::v(0.0f, 0.0f) },
        { core::bbox_2d(core::v(5.0f, 10.0f),  core::v(-5.0f, 0.0f)),  core::v(0.0f, 5.0f) },
        { core::bbox_2d(core::v(10.0f, 5.0f),  core::v(0.0f, -5.0f)),  core::v(5.0f, 0.0f) },

        // Same min and max
        { core::bbox_2d(core::v(5.0f, 5.0f),   core::v(5.0f, 5.0f)),   core::v(5.0f, 5.0f) },
        { core::bbox_2d(core::v(-2.0f, 2.0f),  core::v(-2.0f, 2.0f)),  core::v(-2.0f, 2.0f) },
        { core::bbox_2d(core::v(-2.0f, -2.0f), core::v(-2.0f, -2.0f)), core::v(-2.0f, -2.0f) },
        { core::bbox_2d(core::v(2.0f, -2.0f),  core::v(2.0f, -2.0f)),  core::v(2.0f, -2.0f) },
    };

    executeTestTable("bbox2D center test case failed at index: ", cases, [&](auto& c, const char* cErr) {
        auto got = c.input.center();
        Assert(got.equals(c.expected, epsilon), cErr);

        f32 x = 0, y = 0;
        c.input.center(x, y);
        Assert(core::safe_eq(x, c.expected.x(), epsilon), cErr);
        Assert(core::safe_eq(y, c.expected.y(), epsilon), cErr);
    });

    return 0;
}

constexpr i32 bbox2D_width_and_height_test() {
    constexpr f32 epsilon = 0.0001f;

    struct TestCase {
        core::vec2f min;
        core::vec2f max;
        f32 expctedWidth;
        f32 expctedHeight;
    };

    constexpr TestCase cases[] = {
        { core::v(0.0f, 0.0f),   core::v(10.0f, 10.0f), 10.0f, 10.0f },
        { core::v(-5.0f, -5.0f), core::v(5.0f, 5.0f),   10.0f, 10.0f },
        { core::v(-5.0f, 0.0f),  core::v(5.0f, 10.0f),  10.0f, 10.0f },
        { core::v(0.0f, -5.0f),  core::v(10.0f, 5.0f),  10.0f, 10.0f },

        { core::v(2.0f, 0.0f), core::v(10.0f, 10.0f), 8.0f, 10.0f },
        { core::v(0.0f, 2.0f), core::v(10.0f, 10.0f), 10.0f, 8.0f },

        // Flip min and max
        { core::v(10.0f, 10.0f), core::v(2.0f, 0.0f), -8.0f, -10.0f },
        { core::v(10.0f, 10.0f), core::v(0.0f, 2.0f), -10.0f, -8.0f },

        // Same min and max
        { core::v(5.0f, 5.0f),   core::v(5.0f, 5.0f),   0.0f, 0.0f },
        { core::v(-2.0f, 2.0f),  core::v(-2.0f, 2.0f),  0.0f, 0.0f },
        { core::v(-2.0f, -2.0f), core::v(-2.0f, -2.0f), 0.0f, 0.0f },
        { core::v(2.0f, -2.0f),  core::v(2.0f, -2.0f),  0.0f, 0.0f },
    };

    executeTestTable("bbox2D width and height test case failed at index: ", cases, [&](auto& c, const char* cErr) {
        auto got = core::bbox_2d(c.min, c.max);
        Assert(core::safe_eq(got.width(), c.expctedWidth, epsilon), cErr);
        Assert(core::safe_eq(got.height(), c.expctedHeight, epsilon), cErr);
    });

    return 0;
}

constexpr i32 bbox2D_is_inside_test() {
    struct TestCase {
        core::vec2f min;
        core::vec2f max;
        core::vec2f point;
        bool expected;
    };

    constexpr TestCase cases[] = {
        { core::v(0.0f, 0.0f),         core::v(10.0f, 10.0f),         core::v(5.0f, 5.0f),   true },
        { core::v(0.0f, 0.0f),         core::v(10.00001f, 10.00001f), core::v(10.0f, 10.0f), true },
        { core::v(-0.0001f, -0.0001f), core::v(10.0f, 10.0f),         core::v(0.0f, 0.0f),   true },
        { core::v(0.0f, 0.0f),         core::v(10.0f, 10.0f),         core::v(10.0f, 10.0f), true },

        { core::v(0.0f, 0.0f), core::v(10.0f, 10.0f), core::v(0.0f, 10.0001f),   false },
        { core::v(0.0f, 0.0f), core::v(10.0f, 10.0f), core::v(10.0001f, 0.0f),   false },
        { core::v(0.0f, 0.0f), core::v(10.0f, 10.0f), core::v(-0.0001f, 0.0f),   false },
        { core::v(0.0f, 0.0f), core::v(10.0f, 10.0f), core::v(0.0f, -0.0001f),   false },
        { core::v(0.0f, 0.0f), core::v(10.0f, 10.0f), core::v(10.00001f, 10.0f), false },
        { core::v(0.0f, 0.0f), core::v(10.0f, 10.0f), core::v(10.0f, 10.00001f), false },

        // Flip min and max. This will likely return false for every point value.
        // IMPORTANT: This is undefined behaviour!
        // { core::v(10.0f, 10.0f),         core::v(0.0f, 0.0f),         core::v(5.0f, 5.0f),   false },
        // { core::v(10.00001f, 10.00001f), core::v(0.0f, 0.0f),         core::v(10.0f, 10.0f), false },
        // { core::v(10.0f, 10.0f),         core::v(-0.0001f, -0.0001f), core::v(0.0f, 0.0f),   false },
        // { core::v(10.0f, 10.0f),         core::v(0.0f, 0.0f),         core::v(10.0f, 10.0f), false },
    };

    executeTestTable("bbox2D is inside test case failed at index: ", cases, [&](auto& c, const char* cErr) {
        auto got = core::bbox_2d(c.min, c.max);
        Assert(got.is_inside(c.point) == c.expected, cErr);
    });

    return 0;
}

constexpr i32 bbox2D_conv_test() {
    constexpr f32 epsilon = 0.0001f;

    struct TestCase {
        core::bbox_2d from;
        core::bbox_2d to;
        core::vec2f point;
        core::vec2f expected;
    };

    constexpr TestCase cases[] = {
        {
            core::bbox_2d(core::v(0.f, 0.f), core::v(15.f, 15.f)),
            core::bbox_2d(core::v(0.f, 0.f), core::v(5.f, 5.f)),
            core::v(7.5f, 7.5f),
            core::v(2.5f, 2.5f),
        },
        {
            core::bbox_2d(core::v(5.f, 5.f), core::v(10.f, 10.f)),
            core::bbox_2d(core::v(0.f, 0.f), core::v(15.f, 15.f)),
            core::v(7.5f, 7.5f),
            core::v(7.5f, 7.5f),
        },
        {
            core::bbox_2d(core::v(0.f, 0.f), core::v(10.f, 10.f)),
            core::bbox_2d(core::v(-1.f, -1.f), core::v(1.f, 3.f)),
            core::v(5.f, 5.f),
            core::v(0.f, 1.f),
        },
        {
            core::bbox_2d(core::v(0.f, 0.f), core::v(100.f, 100.f)),
            core::bbox_2d(core::v(0.f, 0.f), core::v(10.f, 10.f)),
            core::v(99.f, 99.f),
            core::v(9.9f, 9.9f),
        },
        {
            core::bbox_2d(core::v(0.f, 0.f), core::v(100.f, 100.f)),
            core::bbox_2d(core::v(0.f, 0.f), core::v(10.f, 10.f)),
            core::v(100.f, 100.f),
            core::v(10.f, 10.f),
        },
        {
            core::bbox_2d(core::v(0.f, 0.f), core::v(100.f, 100.f)),
            core::bbox_2d(core::v(0.f, 0.f), core::v(10.f, 10.f)),
            core::v(0.f, 0.f),
            core::v(0.f, 0.f),
        },

        // Flip min and max.
        {
            core::bbox_2d(core::v(15.f, 15.f), core::v(0.f, 0.f)),
            core::bbox_2d(core::v(5.f, 5.f), core::v(0.f, 0.f)),
            core::v(7.5f, 7.5f),
            core::v(2.5f, 2.5f),
        },
        {
            core::bbox_2d(core::v(10.f, 10.f), core::v(5.f, 5.f)),
            core::bbox_2d(core::v(15.f, 15.f), core::v(0.f, 0.f)),
            core::v(7.5f, 7.5f),
            core::v(7.5f, 7.5f),
        },
        {
            core::bbox_2d(core::v(10.f, 10.f), core::v(0.f, 0.f)),
            core::bbox_2d(core::v(1.f, 3.f), core::v(-1.f, -1.f)),
            core::v(5.f, 5.f),
            core::v(0.f, 1.f),
        },
        {
            core::bbox_2d(core::v(100.f, 100.f), core::v(0.f, 0.f)),
            core::bbox_2d(core::v(10.f, 10.f), core::v(0.f, 0.f)),
            core::v(99.f, 99.f),
            core::v(9.9f, 9.9f),
        },
        {
            core::bbox_2d(core::v(100.f, 100.f), core::v(0.f, 0.f)),
            core::bbox_2d(core::v(10.f, 10.f), core::v(0.f, 0.f)),
            core::v(100.f, 100.f),
            core::v(10.f, 10.f),
        },
        {
            core::bbox_2d(core::v(0.f, 0.f), core::v(10.f, 10.f)),
            core::bbox_2d(core::v(0.f, 0.f), core::v(100.f, 100.f)),
            core::v(0.f, 0.f),
            core::v(0.f, 0.f),
        },
    };

    executeTestTable("bbox2D conversion test case failed at index: ", cases, [&](auto& c, const char* cErr) {
        auto got = c.from.conv_to(c.point, c.to);
        Assert(got.equals(c.expected, epsilon), cErr);
    });

    return 0;
}

i32 run_bbox_tests_suite() {
    RunTest(bbox2D_center_test);
    RunTest(bbox2D_width_and_height_test);
    RunTest(bbox2D_is_inside_test);
    RunTest(bbox2D_conv_test);

    // TODO2: These tests are a bit involved. Remainder to do them at some point.
    // RunTest(bbox2D_intersection_with_line_test);
    // RunTest(bbox2D_intersection_with_line_segment_test);

    return 0;
}

constexpr i32 run_compiletime_bbox_tests_suite() {
    RunTestCompileTime(bbox2D_center_test);
    RunTestCompileTime(bbox2D_width_and_height_test);
    RunTestCompileTime(bbox2D_is_inside_test);
    RunTestCompileTime(bbox2D_conv_test);

    return 0;
}
