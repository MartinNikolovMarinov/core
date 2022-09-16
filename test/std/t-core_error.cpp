struct _FatStrErrorImpl {
    using Error = core::error::Error;

    Error e;

    bool         IsErr() const { return e.IsErr(); }
    const Error& Err()   const { return e; }
};

void ErrorInterfaceUsageTest() {
    static constexpr std::string_view testErrMsg = "Error message";
    _FatStrErrorImpl fatStrError;
    Assert(!core::io::IsErr(fatStrError));
    fatStrError.e = testErrMsg;
    Assert(core::io::IsErr(fatStrError));
    Assert(core::io::Err(fatStrError).Err() == testErrMsg);
}

void CoreErrorTestSute() {
    RunTest(ErrorInterfaceUsageTest);
}
