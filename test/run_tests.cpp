
#include "allocator_bump_static.cpp"

// Order matters for these:
#include "t-arr.cpp"
#include "t-cmd_flag.cpp"
#include "t-str_builder.cpp"

#include "t-alloc_bump.cpp"
#include "t-tuple.cpp"
#include "t-rnd.cpp"
#include "t-bits.cpp"
#include "t-ints.cpp"
#include "t-rune.cpp"
#include "t-expected.cpp"
#include "t-core_math.cpp"
#include "t-traits.cpp"
#include "t-cptr_ptr.cpp"
#include "t-char_ptr_conv.cpp"
#include "t-intrinsic.cpp"
#include "t-defer.cpp"
#include "t-static_arr.cpp"
#include "t-mem.cpp"
#include "t-hash.cpp"

i32 run_all_tests() {
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
    RunTestSuite(run_char_ptr_conv_tests_suite);
    RunTestSuite(run_intrinsics_tests_suite);
    RunTestSuite(run_defer_tests_suite);
    RunTestSuite(run_static_arr_tests_suite);
    RunTestSuite(run_mem_tests_suite);
    RunTestSuite(run_hash_tests_suite);

    return 0;
}
