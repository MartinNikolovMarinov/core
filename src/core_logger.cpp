#include <core_logger.h>

#include <core_mem.h>
#include <core_ansi_escape_codes.h>
#include <core_exec_ctx.h>

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
    core::memcopy(tagTranslationTable[tagTranslationTableCount], tag.data(), tag.len());
    tagTranslationTableCount++;
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

    char* buffer = loggingBuffer;
    addr_size bufferSize = BUFFER_SIZE;

    buffer[0] = '\0';

    i32 n;
    {
        va_list args;
        va_start(args, format);
        n = vsnprintf(buffer, bufferSize, format, args);
        va_end(args);
    }

    // Verify vsnprintf was successful.
    {
        if (n < 0) {
            Panic(false, "Failed to format log message.\n");
            return false;
        }
        else if (addr_size(n) >= BUFFER_SIZE) {
            // If the pre-allocated static buffer is not enough, allocate dynamic memory.
            logWarn("Using dynamic memory for logging!");

            bufferSize = addr_size(n);
            buffer = reinterpret_cast<char*>(core::alloc(bufferSize, sizeof(char)));

            va_list args;
            va_start(args, format);
            n = vsnprintf(buffer, bufferSize, format, args);
            va_end(args);

            Panic(addr_size(n) != bufferSize - 1, "Failed a sanity check.\n");
        }
    }

    defer {
        // If dynamic memory was allocated, free it.
        if (bufferSize != BUFFER_SIZE) {
            core::free(buffer, bufferSize, sizeof(char));
        }
    };

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
        printHandler("%s\n", buffer);
        printHandler("%s\n", separator);
    }
    else {
        printHandler(" _fn_(%s): %s\n", funcName, buffer);
    }

    return true;
}

bool logf(const char* format, ...) {
    if (muted) return false;

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    return true;
}

} // namespace core
