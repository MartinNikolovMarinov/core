#include "std_allocator_static.cpp"

#include "t-alloc_std.cpp"
#include "t-plt.cpp"
#include "t-vec.cpp"
#include "t-fs.cpp"
#include "t-transforms.cpp"

void run_all_std_tests() {
    RunTestSuite(run_std_allocator_tests_suite);
#if (defined(OS_WIN) && OS_WIN == 0)
    RunTestSuite(run_plt_tests_suite);
#endif
    RunTestSuite(run_vec_tests_suite);
    RunTestSuite(run_fs_tests_suite);
    RunTestSuite(run_transforms_tests_suite);
}
