i32 calling_stacktrace_does_not_crash() {
    std::string trace = core::stacktrace(20);
    Assert(trace.size() > 0);
    return 0;
}

i32 stacktrace_contains_this_function() {
    std::string trace = core::stacktrace(3);
    Assert(trace.find("stacktrace_contains_this_function") != std::string::npos);
    Assert(trace.find("run_stacktrace_tests_suite") != std::string::npos);
    return 0;
}

inline i32 stacktrace_on_inlined() {
    std::string trace = core::stacktrace(3);
    // in release mode this function will most likely be inlined, so just check for the caller:
    Assert(trace.find("run_stacktrace_tests_suite") != std::string::npos);
    return 0;
}

i32 run_stacktrace_tests_suite() {
#if (defined(OS_WIN) && OS_WIN == 0) // FIXME: uncomment when done.
    RunTest(calling_stacktrace_does_not_crash);
    RunTest(stacktrace_contains_this_function);
    RunTest(stacktrace_on_inlined);
#endif
    return 0;
}
