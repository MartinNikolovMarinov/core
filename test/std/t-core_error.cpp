struct _FatStrErrorImpl {
    using Error = core::error::Error;

    Error e;

    bool         IsErr() const { return e.IsErr(); }
    const Error& Err()   const { return e; }

    template<typename TRes> friend bool  core::io::IsErr(const TRes& res) noexcept;
    template<typename TRes> friend Error core::io::Err(const TRes& res)   noexcept;
};

namespace core::io
{

template<> bool               IsErr(const _FatStrErrorImpl& res) { return res.IsErr(); }
template<> core::error::Error Err(const _FatStrErrorImpl& res)   { return res.Err(); }

} // namespace core::error

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
