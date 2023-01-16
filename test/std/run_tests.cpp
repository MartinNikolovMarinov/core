#include "std_allocator_static.cpp"

#include "t-alloc_std.cpp"
#include "t-plt.cpp"
#include "t-vec.cpp"

void run_all_std_tests() {
    RunTestSuite(run_std_allocator_tests_suite);
    RunTestSuite(run_plt_tests_suite);
    RunTestSuite(run_vec_tests_suite);
}
