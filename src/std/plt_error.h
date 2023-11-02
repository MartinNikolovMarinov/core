#pragma once

#include <types.h>

namespace core {

using namespace coretypes;

using plt_err_code = i64;

namespace detail {

constexpr plt_err_code ERR_START_OF_CUSTOM_ERRORS = plt_err_code(0xffffffff00000000);

constexpr plt_err_code ERR_OS_CUSTOM_ERROR_START = detail::ERR_START_OF_CUSTOM_ERRORS;
constexpr plt_err_code ERR_THREAD_CUSTOM_ERROR_START = detail::ERR_START_OF_CUSTOM_ERRORS + 200;

} // namespace detail


// Custom library errors start from this value.
constexpr plt_err_code ERR_OS_DEALLOC_NULL_ADDR_ERR = detail::ERR_OS_CUSTOM_ERROR_START + 0;
constexpr plt_err_code ERR_OS_UNSUPPORTED_SIGNAL = detail::ERR_OS_CUSTOM_ERROR_START + 1;
constexpr plt_err_code ERR_OS_SYSTEM_NULL_COMMAND = detail::ERR_OS_CUSTOM_ERROR_START + 2;
constexpr plt_err_code ERR_OS_SYSTEM_EXECUTION_FAILED = detail::ERR_OS_CUSTOM_ERROR_START + 3;

constexpr plt_err_code ERR_THREADING_THREAD_IS_NOT_STARTED = detail::ERR_THREAD_CUSTOM_ERROR_START + 0;
constexpr plt_err_code ERR_THREADING_THREAD_IS_ALREADY_RUNNING = detail::ERR_THREAD_CUSTOM_ERROR_START + 1;
constexpr plt_err_code ERR_THREADING_INVALID_MUTEX_TYPE = detail::ERR_THREAD_CUSTOM_ERROR_START + 2;
constexpr plt_err_code ERR_THREADING_INVALID_THREAD_NAME = detail::ERR_THREAD_CUSTOM_ERROR_START + 3;
constexpr plt_err_code ERR_THREADING_MISSING_TLS_VALUE = detail::ERR_THREAD_CUSTOM_ERROR_START + 4;

constexpr char const* custom_plt_err_code_to_cptr(plt_err_code err) {
    if (err < detail::ERR_START_OF_CUSTOM_ERRORS) {
        return "Not a custom user error";
    }

    switch (err) {
        case ERR_OS_DEALLOC_NULL_ADDR_ERR:   return "Deallocating a null address";
        case ERR_OS_UNSUPPORTED_SIGNAL:      return "Unsupported signal";
        case ERR_OS_SYSTEM_NULL_COMMAND:     return "System command is null";
        case ERR_OS_SYSTEM_EXECUTION_FAILED: return "System command execution failed";

        case ERR_THREADING_THREAD_IS_NOT_STARTED:     return "Thread is not started";
        case ERR_THREADING_THREAD_IS_ALREADY_RUNNING: return "Thread is already running";
        case ERR_THREADING_INVALID_THREAD_NAME:       return "Invalid thread name";
        case ERR_THREADING_INVALID_MUTEX_TYPE:        return "Invalid mutex type";
        case ERR_THREADING_MISSING_TLS_VALUE:         return "Missing TLS value";
    }

    return "Unknown error";
}

} // namespace core
