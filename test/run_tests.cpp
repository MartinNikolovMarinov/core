#include "allocator_bump_static.cpp"

// NOTE: order matters
#include "t-tuple.cpp"
#include "t-rnd.cpp"
#include "t-bits.cpp"
#include "t-ints.cpp"
#include "t-rune.cpp"
#include "t-arr.cpp"
#include "t-alloc_bump.cpp"
#include "t-expected.cpp"

void run_all_tests() {
    RunTestSuite(run_tuple_tests_suite);
    RunTestSuite(run_rnd_tests_suite);
    RunTestSuite(run_bits_tests_suite);
    RunTestSuite(run_ints_tests_suite);
    RunTestSuite(run_rune_tests_suite);
    RunTestSuite(run_bump_allocator_tests_suite);
    RunTestSuite(run_expected_tests_suite);
}
