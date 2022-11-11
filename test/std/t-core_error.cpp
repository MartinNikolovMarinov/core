#include <core_error.h>

struct _FatStrErrorImpl {
    using Error = core::error::Error;

    Error e;

    bool         IsErr() const { return e.IsErr(); }
    const Error& Err()   const { return e; }
};

void ErrorInterfaceUsageTest() {
    static constexpr std::string_view testErrMsg = "Error message";
    _FatStrErrorImpl fatStrError;
    Assert(!core::IsErr(fatStrError));
    fatStrError.e = testErrMsg;
    Assert(core::IsErr(fatStrError));
    Assert(core::Err(fatStrError).Err() == testErrMsg);
}

void CoreErrorTestSute() {
    RunTest(ErrorInterfaceUsageTest);
}
