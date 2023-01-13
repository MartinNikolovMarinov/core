#include "t-tuple.cpp"
#include "t-rnd.cpp"
#include "t-bits.cpp"
#include "t-ints.cpp"
#include "t-rune.cpp"

void run_all_tests() {
    RunTestSuite(run_tuple_tests_suite);
    RunTestSuite(run_rnd_tests_suite);
    RunTestSuite(run_bits_tests_suite);
    RunTestSuite(run_ints_tests_suite);
    RunTestSuite(run_rune_tests_suite);
}
