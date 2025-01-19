#include <core_logger.h>

#include <core_mem.h>
#include <core_ansi_escape_codes.h>
#include <core_exec_ctx.h>

#include <stdio.h>
#include <stdarg.h>

namespace core {

namespace {

inline auto& allocator = getAllocator(DEFAULT_ALLOCATOR_ID);

constexpr addr_size BUFFER_SIZE = core::CORE_KILOBYTE * 32;
thread_local static char loggingBuffer[BUFFER_SIZE];

LogLevel minimumLogLevel = LogLevel::L_INFO;
bool ignoredTagIndices[MAX_NUMBER_OF_TAGS] = {};
bool muted = false;
bool useAnsi = true;

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
    auto tagIndicesToIgnore = createInfo.tagIndicesToIgnore;

    for (addr_size i = 0; i < tagIndicesToIgnore.len(); ++i) {
        Panic(tagIndicesToIgnore[i] < i32(MAX_NUMBER_OF_TAGS), "Trying to ignore tag index that is out of range.");
        ignoredTagIndices[addr_size(tagIndicesToIgnore[i])] = true;
    }

    if (createInfo.print != nullptr) {
        printHandler = createInfo.print;
    }

    useAnsi = createInfo.useAnsi;

    return true;
}

bool setLoggerTag(i32 idx, core::StrView tag) {
    Panic(tag.len() < MAX_TAG_LEN, "Tag is too long.");
    Panic(idx < i32(MAX_NUMBER_OF_TAGS), "Provided Tag index is out of range.");
    Panic(tagTranslationTable[idx][0] == '\0', "Tag index is already taken.");
    core::memcopy(tagTranslationTable[idx], tag.data(), tag.len());
    tagTranslationTableCount++;
    return true;
}

void setLoggerLevel(LogLevel level) {
    minimumLogLevel = level;
}

LogLevel getLogLevel() {
    return minimumLogLevel;
}

void muteLogger(bool mute) {
    muted = mute;
}

void useAnsiInLogger(bool use) {
    useAnsi = use;
}

bool __log(u8 tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* format, ...) {
    if (muted) return false;
    if (level < minimumLogLevel) return false;
    if (tagTranslationTableCount > 0) {
        Panic(tag < MAX_NUMBER_OF_TAGS, "Provided Tag is out of range.");
        Panic(tagTranslationTable[tag][0] != '\0', "No Tag registered with that index.");
        if (ignoredTagIndices[tag]) return false;
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
            Panic(false, "Failed to format log message.");
            return false;
        }
        else if (addr_size(n) >= BUFFER_SIZE) {
            // If the pre-allocated static buffer is not enough, allocate dynamic memory.
            logWarn("Using dynamic memory for logging!");

            bufferSize = addr_size(n);
            buffer = reinterpret_cast<char*>(allocator.alloc(bufferSize, sizeof(char)));

            va_list args;
            va_start(args, format);
            n = vsnprintf(buffer, bufferSize, format, args);
            va_end(args);

            Panic(addr_size(n) != bufferSize - 1, "Failed a sanity check.");
        }
    }

    defer {
        // If dynamic memory was allocated, free it.
        if (bufferSize != BUFFER_SIZE) {
            allocator.free(buffer, bufferSize, sizeof(char));
        }
    };

    // Print Level
    switch (level) {
        case LogLevel::L_DEBUG:
            if (useAnsi) printHandler(ANSI_BOLD("[DEBUG]"));
            else         printHandler("[DEBUG]");
            break;
        case LogLevel::L_INFO:
            if (useAnsi) printHandler(ANSI_BOLD(ANSI_BRIGHT_BLUE("[INFO]")));
            else         printHandler("[INFO]");
            break;
        case LogLevel::L_WARNING:
            if (useAnsi) printHandler(ANSI_BOLD(ANSI_BRIGHT_YELLOW("[WARNING]")));
            else         printHandler("[WARNING]");
            break;
        case LogLevel::L_ERROR:
            if (useAnsi) printHandler(ANSI_BOLD(ANSI_RED("[ERROR]")));
            else         printHandler("[ERROR]");
            break;
        case LogLevel::L_FATAL:
            if (useAnsi) printHandler(ANSI_BOLD(ANSI_BACKGROUND_RED(ANSI_BRIGHT_WHITE("[FATAL]"))));
            else         printHandler("[FATAL]");
            break;
        case LogLevel::L_TRACE:
            if (useAnsi) printHandler(ANSI_BOLD(ANSI_BRIGHT_GREEN("[TRACE]")));
            else         printHandler("[TRACE]");
            break;

        case LogLevel::SENTINEL: [[fallthrough]];
        default:
            printHandler("[UNKNOWN]");
            break;
    }

    // Print Tag
    if (tagTranslationTableCount > 0) {
        if (useAnsi) printHandler("%s[%s]%s", ANSI_BOLD_START(), tagTranslationTable[tag], ANSI_RESET());
        else         printHandler("[%s]", tagTranslationTable[tag]);
    }

    // Print Message
    if (mode == LogSpecialMode::SECTION_TITLE) {
        const char* separator = useAnsi
            ? ANSI_BOLD(ANSI_BRIGHT_WHITE("---------------------------------------------------------------------"))
            : "---------------------------------------------------------------------";
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

bool logDirectStd(const char* format, ...) {
    if (muted) return false;

    va_list args;
    va_start(args, format);
    // TODO2: I need to find a way to write this function in a template with arguments. Because I cant call the print
    //        handler here and that is annoying.
    vprintf(format, args);
    va_end(args);

    return true;
}

} // namespace core
