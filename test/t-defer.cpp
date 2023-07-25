i32 defer_test() {
    {
        i32 a = 0;
        {
            Assert(a == 0);
            defer { a++; };
            Assert(a == 0);

            {
                Assert(a == 0);
                defer { a++; };
                Assert(a == 0);
            }

            Assert(a == 1);
        }
        Assert(a == 2);
    }

    return 0;
}

i32 run_defer_tests_suite() {
    RunTest(defer_test);

    return 0;
}
