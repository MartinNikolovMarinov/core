void translate_test() {
    core::vec2f v = {1.0f, 2.0f};
    core::vec2f t = {3.0f, 4.0f};
    core::translate(v, t);
    Assert(v == core::vec2f(4.0f, 6.0f));

    v = {1.0f, 2.0f};
    t = {-3.0f, -4.0f};
    core::translate(v, t);
    Assert(v == core::vec2f(-2.0f, -2.0f));
}

void scale_test() {
    core::vec2f v = {1.0f, 2.0f};
    core::vec2f s = {3.0f, 4.0f};
    core::scale(v, s);
    Assert(v == core::vec2f(3.0f, 8.0f));

    v = {1.0f, 2.0f};
    s = {-3.0f, -4.0f};
    core::scale(v, s);
    Assert(v == core::vec2f(-3.0f, -8.0f));

    v = {1.0f, 2.0f};
    s = {0.0f, 0.0f};
    core::scale(v, s);
    Assert(v == core::vec2f(0.0f, 0.0f));
}

void rotate2d_test() {
    constexpr core::vec2f origin = {0.0f, 0.0f};

    core::vec2f v = {1.0f, 2.0f};
    core::rotate(v, origin, core::deg_to_rad(0.0f));
    Assert(v.equals(core::vec2f(1.0f, 2.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate(v, origin, core::deg_to_rad(90.0f));
    Assert(v.equals(core::vec2f(-2.0f, 1.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate(v, origin, core::deg_to_rad(180.0f));
    Assert(v.equals(core::vec2f(-1.0f, -2.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate(v, origin, core::deg_to_rad(270.0f));
    Assert(v.equals(core::vec2f(2.0f, -1.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate(v, origin, core::deg_to_rad(360.0f));
    Assert(v.equals(core::vec2f(1.0f, 2.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate_right(v, origin, core::deg_to_rad(0.0f));
    Assert(v.equals(core::vec2f(1.0f, 2.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate_right(v, origin, core::deg_to_rad(90.0f));
    Assert(v.equals(core::vec2f(2.0f, -1.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate_right(v, origin, core::deg_to_rad(180.0f));
    Assert(v.equals(core::vec2f(-1.0f, -2.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate_right(v, origin, core::deg_to_rad(270.0f));
    Assert(v.equals(core::vec2f(-2.0f, 1.0f), 0.0001f));

    v = {1.0f, 2.0f};
    core::rotate_right(v, origin, core::deg_to_rad(360.0f));
    Assert(v.equals(core::vec2f(1.0f, 2.0f), 0.0001f));

    constexpr core::vec2f origin2 = {1.0f, 2.0f};

    v = {1.0f, 2.0f};
    core::rotate(v, origin2, core::deg_to_rad(0.0f));
    Assert(v.equals(core::vec2f(1.0f, 2.0f), 0.0001f));
}

void run_transforms_tests_suite() {
    RunTest(translate_test);
    RunTest(scale_test);
    RunTest(rotate2d_test);
}
