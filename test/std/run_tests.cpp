#include "std_allocator_static.cpp"

#include "t-alloc_std.cpp"
#include "t-vec.cpp"
#include "t-transforms.cpp"
#include "t-mat.cpp"
#include "t-core_math_std.cpp"
#include "t-bbox.cpp"

#if (!defined(OS_WIN) || OS_WIN == 0)
    #include "t-plt.cpp"
    #include "t-fs.cpp"
    #include "t-stacktrace.cpp"
#endif

i32 run_all_std_tests() {
    RunTestSuite(run_std_allocator_tests_suite);
#if (!defined(OS_WIN) || OS_WIN == 0)
    // TODO: [Windows Support] Allow the plt tests to run on Windows.
    RunTestSuite(run_plt_tests_suite);
    RunTest(run_fs_tests_suite);
    RunTestSuite(run_stacktrace_tests_suite);
#endif
    RunTestSuite(run_vec_tests_suite);
    RunTestSuite(run_transforms_tests_suite);
    RunTestSuite(run_mat_tests_suite);
    RunTestSuite(run_core_math_tests_suite_std);
    RunTestSuite(run_bbox_tests_suite);

    return 0;
}
