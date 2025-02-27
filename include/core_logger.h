#pragma once

#include <core_API.h>
#include <core_ansi_escape_codes.h>
#include <core_cstr_format.h>
#include <core_exec_ctx.h>
#include <core_mem.h>
#include <core_str_view.h>
#include <core_types.h>

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

using PrintFunction = void(*)(core::StrView message);

struct LoggerCreateInfo {
    core::Memory<char> loggerMemory;
    core::Memory<i32> tagIndicesToIgnore;
    PrintFunction print;
    bool useAnsi;
    u32 fallbackMultiplier;

    CORE_API_EXPORT static LoggerCreateInfo createDefault();
};

namespace logdetails {

constexpr addr_size MAX_NUMBER_OF_TAGS = 20;
constexpr addr_size MAX_TAG_LEN = 32;

struct LoggerState {
    inline static auto& allocator = core::getAllocator(core::DEFAULT_ALLOCATOR_ID);

    PrintFunction      printHandler;
    bool               useAnsi;
    LogLevel           minimumLogLevel;
    bool               ignoredTagIndices[MAX_NUMBER_OF_TAGS];
    bool               muted;
    core::Memory<char> loggerMemory;
    char               tagTranslationTable[MAX_NUMBER_OF_TAGS][MAX_TAG_LEN];
    i32                tagTranslationTableCount;
    u32                fallbackMultiplier;

    constexpr core::StrView getSectionSeparator() {
        return useAnsi ?
            sv(ANSI_BOLD(ANSI_BRIGHT_WHITE("---------------------------------------------------------------------"))) :
            sv("---------------------------------------------------------------------");
    }
};

CORE_API_EXPORT LoggerState& getLoggerState();

} // namespace logdetails

CORE_API_EXPORT bool initLogger(const LoggerCreateInfo& createInfo = LoggerCreateInfo::createDefault());
CORE_API_EXPORT bool setLoggerTag(i32 idx, core::StrView tag);
CORE_API_EXPORT void setLogLevel(LogLevel level);
CORE_API_EXPORT LogLevel getLogLevel();
CORE_API_EXPORT void muteLogger(bool mute);
CORE_API_EXPORT void useLoggerANSI(bool use);

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

CORE_API_EXPORT void __debug_logBytes(const void *ptr, addr_size size);

template <typename ...Args>
bool logDirectStd(const char* fmt, Args... args) {
    logdetails::LoggerState state = logdetails::getLoggerState();

    if (state.muted) return false;

    auto& memory = state.loggerMemory;
    auto& printHandler = state.printHandler;

    auto fmtRes = core::format(memory, fmt, args...);
    if (fmtRes.hasErr()) {
        Assert(false, core::formatErrorToCStr(fmtRes.err()));
        return false;
    }

    i32 written = fmtRes.value();
    memory[addr_size(written)] = '\0';

    printHandler(core::sv(memory.data(), memory.len()));
    return true;
}

template <typename ...Args>
bool __log(u8 tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* fmt, Args... args) {
    logdetails::LoggerState state = logdetails::getLoggerState();
    auto& muted = state.muted;
    auto& minimumLogLevel = state.minimumLogLevel;
    auto& tagTranslationTableCount = state.tagTranslationTableCount;
    auto& tagTranslationTable = state.tagTranslationTable;
    auto& loggerMemory = state.loggerMemory;
    auto& printHandler = state.printHandler;
    auto& ignoredTagIndices = state.ignoredTagIndices;
    auto& useAnsi = state.useAnsi;
    auto& fallbackMultiplier = state.fallbackMultiplier;

    if (muted) return false;
    if (level < minimumLogLevel) return false;
    if (tagTranslationTableCount > 0) {
        Panic(tag < logdetails::MAX_NUMBER_OF_TAGS, "Provided Tag is out of range.");
        Panic(tagTranslationTable[tag][0] != '\0', "No Tag registered with that index.");
        if (ignoredTagIndices[tag]) return false;
    }

    char* buffer = loggerMemory.data();
    addr_size bufferSize = loggerMemory.len();

    buffer[0] = '\0';

    // Verify format was successful.
    i32 n;
    {
        auto fmtRes = core::format(buffer, i32(bufferSize), fmt, args...);
        if (fmtRes.hasValue()) {
            n = fmtRes.value();
        }
        else if (fmtRes.err() == FormatError::OUT_BUFFER_OVERFLOW) {
            auto logWarnWasOk = logWarn("Logger is taking the SLOW path and uses dynamic memory! "
                                      "Address this by increasing the logger buffer size. "
                                      "Or by calling the log function multiple times.");
            Panic(logWarnWasOk, "The logger is missconfigured.");
            buffer[0] = '\0'; // NOTE: The logWarn call has indirect recursion, so I need to clear the buffer again.

            // TODO: The strategy for fallback dynamic memory allocation should be configurable.
            buffer = reinterpret_cast<char*>(state.allocator.alloc(bufferSize * fallbackMultiplier, sizeof(char)));

            auto fmtSecondTryRes = core::format(buffer, i32(bufferSize), fmt, args...);
            if (fmtSecondTryRes.hasErr()) {
                Panic(false, "Logging failed even after buffer resize");
                return false;
            }

            n = fmtSecondTryRes.value();
        }
        else {
            Panic(false, formatErrorToCStr(fmtRes.err()));
            return false;
        }
    }

    defer {
        // If dynamic memory was allocated, free it.
        if (bufferSize != loggerMemory.len()) {
            state.allocator.free(buffer, bufferSize, sizeof(char));
        }
    };

    if (mode == LogSpecialMode::SECTION_TITLE) {
        printHandler(state.getSectionSeparator());
        printHandler("\n"_sv);
    }

    // Print Level
    switch (level) {
        case LogLevel::L_DEBUG:
            if (useAnsi) printHandler(core::sv(ANSI_BOLD("[DEBUG]")));
            else         printHandler(core::sv("[DEBUG]"));
            break;
        case LogLevel::L_INFO:
            if (useAnsi) printHandler(core::sv(ANSI_BOLD(ANSI_BRIGHT_BLUE("[INFO]"))));
            else         printHandler(core::sv("[INFO]"));
            break;
        case LogLevel::L_WARNING:
            if (useAnsi) printHandler(core::sv(ANSI_BOLD(ANSI_BRIGHT_YELLOW("[WARNING]"))));
            else         printHandler(core::sv("[WARNING]"));
            break;
        case LogLevel::L_ERROR:
            if (useAnsi) printHandler(core::sv(ANSI_BOLD(ANSI_RED("[ERROR]"))));
            else         printHandler(core::sv("[ERROR]"));
            break;
        case LogLevel::L_FATAL:
            if (useAnsi) printHandler(core::sv(ANSI_BOLD(ANSI_BACKGROUND_RED(ANSI_BRIGHT_WHITE("[FATAL]")))));
            else         printHandler(core::sv("[FATAL]"));
            break;
        case LogLevel::L_TRACE:
            if (useAnsi) printHandler(core::sv(ANSI_BOLD(ANSI_BRIGHT_GREEN("[TRACE]"))));
            else         printHandler(core::sv("[TRACE]"));
            break;

        case LogLevel::SENTINEL: [[fallthrough]];
        default:
            printHandler(core::sv("[UNKNOWN]"));
            break;
    }

    // Print Tag
    if (tagTranslationTableCount > 0) {
        if (useAnsi) printHandler(core::sv(ANSI_BOLD_START()));
        printHandler("["_sv);
        printHandler(core::sv(tagTranslationTable[tag]));
        printHandler("]"_sv);
        if (useAnsi) printHandler(core::sv(ANSI_RESET()));
    }

    // Print Message
    if (mode == LogSpecialMode::SECTION_TITLE) {
        printHandler(" _fn_("_sv);
        printHandler(core::sv(funcName));
        printHandler("):\n"_sv);
        printHandler(core::sv(buffer, addr_size(n)));
        printHandler("\n"_sv);
        printHandler(state.getSectionSeparator());
        printHandler("\n"_sv);
    }
    else {
        printHandler(" _fn_("_sv);
        printHandler(core::sv(funcName));
        printHandler("): "_sv);
        printHandler(core::sv(buffer, addr_size(n)));
        printHandler("\n"_sv);
    }

    return true;
}

} // namespace core
