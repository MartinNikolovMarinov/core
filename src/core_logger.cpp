#include <core_logger.h>

#include <core_mem.h>
#include <core_ansi_escape_codes.h>

#include <stdio.h>
#include <stdarg.h>

namespace core {

namespace {

constexpr addr_size BUFFER_SIZE = core::CORE_KILOBYTE * 32;
thread_local static char loggingBuffer[BUFFER_SIZE];

LogLevel minimumLogLevel = LogLevel::L_INFO;
bool ignoredTagsTable[MAX_NUMBER_OF_TAGS] = {};
bool muted = false;

char tagTranslationTable[MAX_NUMBER_OF_TAGS][MAX_TAG_LEN] = {};
i32 tagTranslationTableCount = 0;

void defaultPrint(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

PrintFunction printHandler = defaultPrint;

} // namespace

bool initLogger(const LoggerCreateInfo& createInfo) {
    auto tagsToIgnore = createInfo.tagsToIgnore;
    auto tagsToIgnoreCount = createInfo.tagsToIgnoreCount;

    if (tagsToIgnore != nullptr) {
        for (addr_size i = 0; i < tagsToIgnoreCount; ++i) {
            ignoredTagsTable[addr_size(tagsToIgnore[i])] = true;
        }
    }

    if (createInfo.print != nullptr) {
        printHandler = createInfo.print;
    }

    return true;
}

bool addTag(core::StrView tag) {
    Panic(tagTranslationTableCount < i32(MAX_NUMBER_OF_TAGS), "Cannot add more tags to the logger.");
    Panic(tag.len() < MAX_TAG_LEN, "Tag is too long.");
    core::memcopy(tagTranslationTable + tagTranslationTableCount, tag.data(), tag.len());
    ++tagTranslationTableCount;
    return true;
}

void setLogLevel(LogLevel level) {
    minimumLogLevel = level;
}

void muteLogger(bool mute) {
    muted = mute;
}

bool __log(u8 tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* format, ...) {
    if (muted) return false;
    if (level < minimumLogLevel) return false;
    if (tagTranslationTableCount > 0) {
        if (ignoredTagsTable[tag]) return false;
        Panic(tagTranslationTableCount > tag, "Tag is not in the translation table.");
    }

    loggingBuffer[0] = '\0';

    va_list args;
    va_start(args, format);
    vsnprintf(loggingBuffer, BUFFER_SIZE, format, args);
    va_end(args);

    // Print Level
    switch (level) {
        case LogLevel::L_DEBUG:
            printHandler(ANSI_BOLD("[DEBUG]"));
            break;
        case LogLevel::L_INFO:
            printHandler(ANSI_BOLD(ANSI_BRIGHT_BLUE("[INFO]")));
            break;
        case LogLevel::L_WARNING:
            printHandler(ANSI_BOLD(ANSI_BRIGHT_YELLOW("[WARNING]")));
            break;
        case LogLevel::L_ERROR:
            printHandler(ANSI_BOLD(ANSI_RED("[ERROR]")));
            break;
        case LogLevel::L_FATAL:
            printHandler(ANSI_BOLD(ANSI_BACKGROUND_RED(ANSI_BRIGHT_WHITE("[FATAL]"))));
            break;
        case LogLevel::L_TRACE:
            printHandler(ANSI_BOLD(ANSI_BRIGHT_GREEN("[TRACE]")));
            break;

        case LogLevel::SENTINEL: [[fallthrough]];
        default:
            printHandler("[UNKNOWN]");
            break;
    }

    // Print Tag
    if (tagTranslationTableCount > 0) {
        printHandler("%s[%s]%s", ANSI_BOLD_START(), tagTranslationTable[tag], ANSI_RESET());
    }

    // Print Message
    if (mode == LogSpecialMode::SECTION_TITLE) {
        constexpr const char* separator = ANSI_BOLD(ANSI_BRIGHT_WHITE("---------------------------------------------------------------------"));
        printHandler(" _fn_(%s):\n", funcName);
        printHandler("%s\n", separator);
        printHandler("%s\n", loggingBuffer);
        printHandler("%s\n", separator);
    }
    else {
        printHandler(" _fn_(%s): %s\n", funcName, loggingBuffer);
    }

    return true;
}

} // namespace core
