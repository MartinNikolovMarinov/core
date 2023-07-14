
#include "allocator_bump_static.cpp"

// Order matters here!
#include "t-arr.cpp"
#include "t-tuple.cpp"
#include "t-rnd.cpp"
#include "t-bits.cpp"
#include "t-ints.cpp"
#include "t-rune.cpp"
#include "t-alloc_bump.cpp"
#include "t-expected.cpp"
#include "t-core_math.cpp"
#include "t-traits.cpp"
#include "t-cptr_ptr.cpp"

int run_all_tests() {
    RunTestSuite(run_tuple_tests_suite);
    RunTestSuite(run_rnd_tests_suite);
    RunTestSuite(run_bits_tests_suite);
    RunTestSuite(run_ints_tests_suite);
    RunTestSuite(run_rune_tests_suite);
    RunTestSuite(run_bump_allocator_tests_suite);
    RunTestSuite(run_expected_tests_suite);
    RunTestSuite(run_core_math_tests_suite);
    RunTestSuite(run_traits_tests_suite);
    RunTestSuite(run_cptr_ptr_tests_suite);

    return 0;
}
