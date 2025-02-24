#pragma once

// namespace detail {

//     LogLevel minimumLogLevel = LogLevel::L_INFO;
//     bool ignoredTagIndices[MAX_NUMBER_OF_TAGS] = {};
//     bool muted = false;
//     bool useAnsi = true;

//     PrintFunction printHandler = defaultPrint;
// };

// LoggerState g_loggerState;

// } // namespace detail

// CORE_API_EXPORT bool __log(u8 tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* format, ...);
// // CORE_API_EXPORT bool logDirectStd(const char* format, ...);

// #define logTrace(format, ...) __log(0, core::LogLevel::L_TRACE,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logDebug(format, ...) __log(0, core::LogLevel::L_DEBUG,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logInfo(format, ...)  __log(0, core::LogLevel::L_INFO,    core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logWarn(format, ...)  __log(0, core::LogLevel::L_WARNING, core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logErr(format, ...)   __log(0, core::LogLevel::L_ERROR,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logFatal(format, ...) __log(0, core::LogLevel::L_FATAL,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)

// #define logTraceTagged(tag, format, ...) __log(tag, core::LogLevel::L_TRACE,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logDebugTagged(tag, format, ...) __log(tag, core::LogLevel::L_DEBUG,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logInfoTagged(tag, format, ...)  __log(tag, core::LogLevel::L_INFO,    core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logWarnTagged(tag, format, ...)  __log(tag, core::LogLevel::L_WARNING, core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logErrTagged(tag, format, ...)   __log(tag, core::LogLevel::L_ERROR,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
// #define logFatalTagged(tag, format, ...) __log(tag, core::LogLevel::L_FATAL,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)

// #define logSectionTitleTraceTagged(tag, format, ...) __log(tag, core::LogLevel::L_TRACE,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
// #define logSectionTitleDebugTagged(tag, format, ...) __log(tag, core::LogLevel::L_DEBUG,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
// #define logSectionTitleInfoTagged(tag, format, ...)  __log(tag, core::LogLevel::L_INFO,    core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
// #define logSectionTitleWarnTagged(tag, format, ...)  __log(tag, core::LogLevel::L_WARNING, core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
// #define logSectionTitleErrTagged(tag, format, ...)   __log(tag, core::LogLevel::L_ERROR,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
// #define logSectionTitleFatalTagged(tag, format, ...) __log(tag, core::LogLevel::L_FATAL,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)

// inline void __debug_logBytes(const void *ptr, addr_size size) {
//     const u8* bytePtr = reinterpret_cast<const u8*>(ptr);
//     for (addr_off i = addr_off(size) - 1; i >= 0; i--) {
//         logDirectStd("%02x ", bytePtr[i]);
//     }
// }

// } // namespace core

#include <core_API.h>
#include <core_types.h>
#include <core_str_view.h>
#include <core_cstr_format.h>
#include <core_mem.h>

#include <stdio.h>
#include <stdarg.h>

namespace core {

using namespace coretypes;

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

using PrintFunction = void(*)(const char* message);

struct LoggerCreateInfo {
    core::Memory<char> loggerMemory;
    core::Memory<i32> tagIndicesToIgnore;
    PrintFunction print = nullptr;
    bool useAnsi = true;

    CORE_API_EXPORT static LoggerCreateInfo createDefault();
};

namespace logdetails {

constexpr addr_size MAX_NUMBER_OF_TAGS = 20;
constexpr addr_size MAX_TAG_LEN = 32;

// Global State
extern PrintFunction      printHandler;
extern bool               useAnsi;
extern LogLevel           minimumLogLevel;
extern bool               ignoredTagIndices[MAX_NUMBER_OF_TAGS];
extern bool               muted;
extern core::Memory<char> loggerMemory;
extern char               tagTranslationTable[MAX_NUMBER_OF_TAGS][MAX_TAG_LEN];
extern i32                tagTranslationTableCount;

} // namespace logdetails

CORE_API_EXPORT bool initLogger(const LoggerCreateInfo& createInfo = LoggerCreateInfo::createDefault());
CORE_API_EXPORT bool setLoggerTag(i32 idx, core::StrView tag);
CORE_API_EXPORT void setLogLevel(LogLevel level);
CORE_API_EXPORT LogLevel getLogLevel();
CORE_API_EXPORT void muteLogger(bool mute);
CORE_API_EXPORT void useLoggerANSI(bool use);

template <typename ...Args>
bool logDirectStd(const char* fmt, Args... args) {
    using namespace logdetails;

    if (muted) return false;

    auto& memory = loggerMemory;

    auto fmtRes = core::format(memory, fmt, args...);
    if (fmtRes.hasErr()) {
        Assert(false, core::formatErrorToCStr(fmtRes.err()));
        return false;
    }

    auto written = fmtRes.value();
    memory[written] = '\0';

    printHandler(memory.data());
    return true;
}

// bool __log(u8 tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* format, ...) {
//     if (muted) return false;
//     if (level < minimumLogLevel) return false;
//     if (tagTranslationTableCount > 0) {
//         Panic(tag < MAX_NUMBER_OF_TAGS, "Provided Tag is out of range.");
//         Panic(tagTranslationTable[tag][0] != '\0', "No Tag registered with that index.");
//         if (ignoredTagIndices[tag]) return false;
//     }

//     char* buffer = loggingBuffer;
//     addr_size bufferSize = BUFFER_SIZE;

//     buffer[0] = '\0';

//     i32 n;
//     {
//         va_list args;
//         va_start(args, format);
//         n = vsnprintf(buffer, bufferSize, format, args);
//         va_end(args);
//     }

//     // Verify vsnprintf was successful.
//     {
//         if (n < 0) {
//             Panic(false, "Failed to format log message.");
//             return false;
//         }
//         else if (addr_size(n) >= BUFFER_SIZE) {
//             // If the pre-allocated static buffer is not enough, allocate dynamic memory.
//             logWarn("Using dynamic memory for logging!");

//             bufferSize = addr_size(n);
//             buffer = reinterpret_cast<char*>(allocator.alloc(bufferSize, sizeof(char)));`

//             va_list args;
//             va_start(args, format);
//             n = vsnprintf(buffer, bufferSize, format, args);
//             va_end(args);

//             Panic(addr_size(n) != bufferSize - 1, "Failed a sanity check.");
//         }
//     }

//     defer {
//         // If dynamic memory was allocated, free it.
//         if (bufferSize != BUFFER_SIZE) {
//             allocator.free(buffer, bufferSize, sizeof(char));
//         }
//     };

//     // Print Level
//     switch (level) {
//         case LogLevel::L_DEBUG:
//             if (useAnsi) printHandler(ANSI_BOLD("[DEBUG]"));
//             else         printHandler("[DEBUG]");
//             break;
//         case LogLevel::L_INFO:
//             if (useAnsi) printHandler(ANSI_BOLD(ANSI_BRIGHT_BLUE("[INFO]")));
//             else         printHandler("[INFO]");
//             break;
//         case LogLevel::L_WARNING:
//             if (useAnsi) printHandler(ANSI_BOLD(ANSI_BRIGHT_YELLOW("[WARNING]")));
//             else         printHandler("[WARNING]");
//             break;
//         case LogLevel::L_ERROR:
//             if (useAnsi) printHandler(ANSI_BOLD(ANSI_RED("[ERROR]")));
//             else         printHandler("[ERROR]");
//             break;
//         case LogLevel::L_FATAL:
//             if (useAnsi) printHandler(ANSI_BOLD(ANSI_BACKGROUND_RED(ANSI_BRIGHT_WHITE("[FATAL]"))));
//             else         printHandler("[FATAL]");
//             break;
//         case LogLevel::L_TRACE:
//             if (useAnsi) printHandler(ANSI_BOLD(ANSI_BRIGHT_GREEN("[TRACE]")));
//             else         printHandler("[TRACE]");
//             break;

//         case LogLevel::SENTINEL: [[fallthrough]];
//         default:
//             printHandler("[UNKNOWN]");
//             break;
//     }

//     // Print Tag
//     if (tagTranslationTableCount > 0) {
//         if (useAnsi) printHandler("%s[%s]%s", ANSI_BOLD_START(), tagTranslationTable[tag], ANSI_RESET());
//         else         printHandler("[%s]", tagTranslationTable[tag]);
//     }

//     // Print Message
//     if (mode == LogSpecialMode::SECTION_TITLE) {
//         const char* separator = useAnsi
//             ? ANSI_BOLD(ANSI_BRIGHT_WHITE("---------------------------------------------------------------------"))
//             : "---------------------------------------------------------------------";
//         printHandler(" _fn_(%s):\n", funcName);
//         printHandler("%s\n", separator);
//         printHandler("%s\n", buffer);
//         printHandler("%s\n", separator);
//     }
//     else {
//         printHandler(" _fn_(%s): %s\n", funcName, buffer);
//     }

//     return true;
// }

} // namespace core

extern int mmcounter;
