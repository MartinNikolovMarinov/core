#pragma once

#include <core_types.h>

namespace core {

using namespace coretypes;

using PltErrCode = i64;

namespace detail {

constexpr PltErrCode ERR_START_OF_CUSTOM_ERRORS = PltErrCode(0xffffffff00000000);

constexpr PltErrCode ERR_OS_CUSTOM_ERROR_START = detail::ERR_START_OF_CUSTOM_ERRORS;
constexpr PltErrCode ERR_THREAD_CUSTOM_ERROR_START = detail::ERR_START_OF_CUSTOM_ERRORS + 200;

} // namespace detail

constexpr PltErrCode ERR_THREADING_INVALID_THREAD_NAME = detail::ERR_THREAD_CUSTOM_ERROR_START + 0;
constexpr PltErrCode ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD = detail::ERR_THREAD_CUSTOM_ERROR_START + 1;
constexpr PltErrCode ERR_THREAD_IS_NOT_JOINABLE = detail::ERR_THREAD_CUSTOM_ERROR_START + 2;

// Custom library errors start from this value.

constexpr char const* customPltErrCodeToCptr(PltErrCode err) {
    if (err < detail::ERR_START_OF_CUSTOM_ERRORS) {
        return "Not a custom user error";
    }

    switch (err) {
        case ERR_THREADING_INVALID_THREAD_NAME:                return "Invalid thread name";
        case ERR_THREADING_STARTING_AN_ALREADY_RUNNING_THREAD: return "Starting an already running thread";
        case ERR_THREAD_IS_NOT_JOINABLE:                       return "Thread is not joinable";
    }

    return "Unknown error";
}

} // namespace core
