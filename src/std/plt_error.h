#pragma once

#include <types.h>

namespace core {

using namespace coretypes;

using plt_err_code = i64;

// Custom library errors start from this value.
constexpr plt_err_code ERR_OS_CUSTOM_ERROR_START = plt_err_code(0xffffffff00000000);
constexpr plt_err_code ERR_OS_DEALLOC_NULL_ADDR_ERR = ERR_OS_CUSTOM_ERROR_START + 1;

constexpr plt_err_code ERR_THREAD_CUSTOM_ERROR_START = plt_err_code(0xffffffff00000200);
constexpr plt_err_code ERR_THREAD_IS_NOT_STARTED = ERR_THREAD_CUSTOM_ERROR_START + 1;
constexpr plt_err_code ERR_THREAD_IS_ALREADY_RUNNING = ERR_THREAD_CUSTOM_ERROR_START + 2;


} // namespace core
