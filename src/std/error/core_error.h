#pragma once

#include <API.h>
#include <types.h>

#include <string_view>
#include <string>

namespace core::error
{

using namespace coretypes;

struct CORE_API_EXPORT Error {
    Error() = default;
    Error(std::string_view msg) : msg(msg) {}
    Error(const Error& other) : msg(other.msg) {}
    Error(Error&& other) : msg(std::move(other.msg)) {}

    Error& operator=(const Error& other) {
        msg = other.msg;
        return *this;
    }

    bool IsErr() const { return !msg.empty(); }

    std::string msg;
};

template<typename T>
struct CORE_API_EXPORT ErrorValue {
    ErrorValue() = default;
    ErrorValue(T&& val, Error&& err) : val(std::move(val)), err(std::move(err)) {}
    ErrorValue(const ErrorValue& other) : val(other.val), err(other.err) {}
    ErrorValue(ErrorValue&& other) : val(std::move(other.val)), err(std::move(other.err)) {}

    bool IsErr() const { return err.IsErr(); }

    T val;
    Error err;
};

} // namespace core::error
