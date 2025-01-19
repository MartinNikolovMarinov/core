#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_str_view.h>

namespace core {

using namespace coretypes;

constexpr addr_size MAX_NUMBER_OF_TAGS = 20;
constexpr addr_size MAX_TAG_LEN = 32;

enum struct LogLevel : u8 {
    L_TRACE = 0,
    L_DEBUG,
    L_INFO,
    L_WARNING,
    L_ERROR,
    L_FATAL,

    SENTINEL
};

enum struct LogSpecialMode : u8 {
    NONE = 0,

    SECTION_TITLE,

    SENTINEL
};

using PrintFunction = void(*)(const char* format, ...);

struct LoggerCreateInfo {
    core::Memory<i32> tagIndicesToIgnore;
    PrintFunction print = nullptr;
    bool useAnsi = true;
};

CORE_API_EXPORT bool initLogger(const LoggerCreateInfo& createInfo);
CORE_API_EXPORT bool setLoggerTag(i32 idx, core::StrView tag);
CORE_API_EXPORT void setLoggerLevel(LogLevel level);
CORE_API_EXPORT LogLevel getLogLevel();
CORE_API_EXPORT void muteLogger(bool mute);
CORE_API_EXPORT void useAnsiInLogger(bool use);

CORE_API_EXPORT bool __log(u8 tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* format, ...);
CORE_API_EXPORT bool logDirectStd(const char* format, ...);

#define logTrace(format, ...) __log(0, core::LogLevel::L_TRACE,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logDebug(format, ...) __log(0, core::LogLevel::L_DEBUG,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logInfo(format, ...)  __log(0, core::LogLevel::L_INFO,    core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logWarn(format, ...)  __log(0, core::LogLevel::L_WARNING, core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logErr(format, ...)   __log(0, core::LogLevel::L_ERROR,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logFatal(format, ...) __log(0, core::LogLevel::L_FATAL,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)

#define logTraceTagged(tag, format, ...) __log(tag, core::LogLevel::L_TRACE,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logDebugTagged(tag, format, ...) __log(tag, core::LogLevel::L_DEBUG,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logInfoTagged(tag, format, ...)  __log(tag, core::LogLevel::L_INFO,    core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logWarnTagged(tag, format, ...)  __log(tag, core::LogLevel::L_WARNING, core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logErrTagged(tag, format, ...)   __log(tag, core::LogLevel::L_ERROR,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logFatalTagged(tag, format, ...) __log(tag, core::LogLevel::L_FATAL,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)

#define logSectionTitleTraceTagged(tag, format, ...) __log(tag, core::LogLevel::L_TRACE,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleDebugTagged(tag, format, ...) __log(tag, core::LogLevel::L_DEBUG,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleInfoTagged(tag, format, ...)  __log(tag, core::LogLevel::L_INFO,    core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleWarnTagged(tag, format, ...)  __log(tag, core::LogLevel::L_WARNING, core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleErrTagged(tag, format, ...)   __log(tag, core::LogLevel::L_ERROR,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleFatalTagged(tag, format, ...) __log(tag, core::LogLevel::L_FATAL,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)

inline void __debug_logBytes(const void *ptr, addr_size size) {
    const u8* bytePtr = reinterpret_cast<const u8*>(ptr);
    for (addr_off i = addr_off(size) - 1; i >= 0; i--) {
        logDirectStd("%02x ", bytePtr[i]);
    }
}

} // namespace core
