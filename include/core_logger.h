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
    L_MUTE,

    SENTINEL
};

enum struct LogSpecialMode : u8 {
    NONE = 0,

    SECTION_TITLE,

    SENTINEL
};

using PrintFunction = void(*)(core::StrView message);

struct LoggerCreateInfo {
    core::AllocatorId allocatorId;
    PrintFunction print;
    bool useAnsi;

    CORE_API_EXPORT static LoggerCreateInfo createDefault();
};

namespace logdetails {

constexpr addr_size MAX_NUMBER_OF_TAGS = 20;
constexpr addr_size MAX_TAG_LEN = 32;

struct LoggerState {
    PrintFunction      printHandler;
    bool               useAnsi;
    LogLevel           minimumLogLevel;
    LogLevel           logLevelPerTag[MAX_NUMBER_OF_TAGS];
    bool               muted;
    core::Memory<char> loggerMemory;
    char               tagTranslationTable[MAX_NUMBER_OF_TAGS][MAX_TAG_LEN]; // NOTE: idx=0 is reserved.
    i32                tagTranslationTableCount;
    AllocatorId        allocatorId;
    bool               isInitialized;

    constexpr core::StrView getSectionSeparator() {
        return useAnsi ?
            sv(ANSI_BOLD(ANSI_BRIGHT_WHITE("---------------------------------------------------------------------"))) :
            sv("---------------------------------------------------------------------");
    }
};

CORE_API_EXPORT LoggerState& getLoggerState();

} // namespace logdetails

[[nodiscard]] CORE_API_EXPORT bool     loggerInit(const LoggerCreateInfo& createInfo = LoggerCreateInfo::createDefault());
              CORE_API_EXPORT void     loggerDestroy();
[[nodiscard]] CORE_API_EXPORT bool     loggerSetTag(i32 idx, core::StrView tag);
              CORE_API_EXPORT void     loggerSetLevel(LogLevel level);
              CORE_API_EXPORT void     loggerSetLevel(LogLevel level, i32 tagIdx);
              CORE_API_EXPORT LogLevel loggerGetLevel();
              CORE_API_EXPORT LogLevel loggerGetLevel(i32 tagIdx);
              CORE_API_EXPORT void     loggerMute(bool mute);
              CORE_API_EXPORT void     loggerUseANSI(bool use);

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
    logdetails::LoggerState& state = logdetails::getLoggerState();

    if (state.muted) return false;

    i32 written = 0;
    while (true) {
        auto fmtRes = core::format(state.loggerMemory, fmt, args...);
        if (!fmtRes.hasErr()) {
            written = fmtRes.value();
            break;
        }
        else if (fmtRes.err() == FormatError::OUT_BUFFER_OVERFLOW) {
            state.loggerMemory = memoryReallocate(
                std::move(state.loggerMemory),
                state.loggerMemory.len() * 2,
                state.allocatorId
            );
        }
        else {
            Panic(false, core::formatErrorToCStr(fmtRes.err()));
            return false;
        }
    }
    state.loggerMemory[addr_size(written)] = '\0';

    // Finally print successfully:
    state.printHandler(core::sv(state.loggerMemory.data(), state.loggerMemory.len()));
    return true;
}

template <typename ...Args>
bool __log(u8 tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* fmt, Args... args) {
    logdetails::LoggerState& state = logdetails::getLoggerState();
    auto& muted = state.muted;
    auto& minimumLogLevel = state.minimumLogLevel;
    auto& tagTranslationTableCount = state.tagTranslationTableCount;
    auto& tagTranslationTable = state.tagTranslationTable;
    auto& printHandler = state.printHandler;
    auto& logLevelPerTag = state.logLevelPerTag;
    auto& useAnsi = state.useAnsi;
    AllocatorId allocatorId = state.allocatorId;

    if (muted) return false;
    if (level < minimumLogLevel) return false;
    if (tagTranslationTableCount > 0) {
        Panic(tag < logdetails::MAX_NUMBER_OF_TAGS, "Provided Tag is out of range.");
        Panic(tagTranslationTable[tag][0] != '\0', "No Tag registered with that index.");
        if (level < logLevelPerTag[tag]) return false;
    }

    state.loggerMemory[0] = '\0';

    // Write until successfull or out of memory.
    i32 written;
    {
        while (true) {
            auto fmtRes = core::format(state.loggerMemory, fmt, args...);
            if (!fmtRes.hasErr()) {
                written = fmtRes.value();
                break;
            }
            else if (fmtRes.err() == FormatError::OUT_BUFFER_OVERFLOW) {
                state.loggerMemory = memoryReallocate(
                    std::move(state.loggerMemory),
                    state.loggerMemory.len() * 2,
                    allocatorId
                );
            }
            else {
                Panic(false, core::formatErrorToCStr(fmtRes.err()));
                return false;
            }
        }
        state.loggerMemory[addr_size(written)] = '\0';
    }

    if (mode == LogSpecialMode::SECTION_TITLE) {
        printHandler(state.getSectionSeparator());
        printHandler("\n"_sv);
    }

    // Print Tag
    if (tagTranslationTableCount > 0 && tag > 0) {
        if (useAnsi) printHandler(core::sv(ANSI_BOLD_START()));
        printHandler("["_sv);
        printHandler(core::sv(tagTranslationTable[tag]));
        printHandler("]"_sv);
        if (useAnsi) printHandler(core::sv(ANSI_RESET()));
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

        case LogLevel::L_MUTE: [[fallthrough]]; // should never be reached
        case LogLevel::SENTINEL: [[fallthrough]];
        default:
            printHandler(core::sv("[UNKNOWN]"));
            break;
    }

    // TODO: The logger needs a unix timestamp.

    // Print Message
    if (mode == LogSpecialMode::SECTION_TITLE) {
        printHandler(" "_sv);
        printHandler(core::sv(state.loggerMemory.data(), addr_size(written)));
        printHandler("\n"_sv);
        printHandler(state.getSectionSeparator());
        printHandler("\n"_sv);
    }
    else {
        printHandler(" _fn_("_sv);
        printHandler(core::sv(funcName));
        printHandler("): "_sv);
        printHandler(core::sv(state.loggerMemory.data(), addr_size(written)));
        printHandler("\n"_sv);
    }

    return true;
}

} // namespace core
