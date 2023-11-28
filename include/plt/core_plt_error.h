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


// Custom library errors start from this value.

constexpr char const* customPltErrCodeToCptr(PltErrCode err) {
    if (err < detail::ERR_START_OF_CUSTOM_ERRORS) {
        return "Not a custom user error";
    }

    // switch (err) {
    // }

    return "Unknown error";
}

} // namespace core
