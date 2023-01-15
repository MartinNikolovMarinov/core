#include "std_allocator_static.cpp"

// NOTE: order matters here.
#include "t-alloc_std.cpp"

void run_all_std_tests() {
    RunTestSuite(run_std_allocator_tests_suite);
}
