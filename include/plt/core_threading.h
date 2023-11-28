#pragma once

#include <core_API.h>
#include <core_expected.h>
#include <core_types.h>
#include <plt/core_plt_error.h>

#include <atomic>

namespace core {

using namespace coretypes;

using AtomicChar = std::atomic<char>;
using AtomicI32  = std::atomic<i32>;
using AtomicI64  = std::atomic<i64>;
using AtomicU32  = std::atomic<u32>;
using AtomicU64  = std::atomic<u64>;
using AtomicBool = std::atomic<bool>;
using AtomicPtr  = std::atomic<void*>;

CORE_API_EXPORT expected<i32, PltErrCode> threadingGetNumCores();

} // namespace core
