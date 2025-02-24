#include <core_logger.h>

#include <core_mem.h>
#include <core_ansi_escape_codes.h>
#include <core_exec_ctx.h>

namespace core {

namespace logdetails {

// Global State
PrintFunction      printHandler = nullptr;
bool               useAnsi = true;
LogLevel           minimumLogLevel = LogLevel::L_INFO;
bool               ignoredTagIndices[MAX_NUMBER_OF_TAGS] = {};
bool               muted = false;
core::Memory<char> loggerMemory = {};
char               tagTranslationTable[MAX_NUMBER_OF_TAGS][MAX_TAG_LEN] = {};
i32                tagTranslationTableCount = 0;

} // logdetails

using namespace logdetails;

LoggerCreateInfo LoggerCreateInfo::createDefault() {
    constexpr static addr_size BUFFER_SIZE = core::CORE_KILOBYTE * 32;
    thread_local static char loggingBuffer[BUFFER_SIZE];

    LoggerCreateInfo ret;
    ret.loggerMemory = { loggingBuffer, BUFFER_SIZE };
    ret.tagIndicesToIgnore = {};
    ret.print = nullptr;
    ret.useAnsi = true;
    return ret;
}

bool initLogger(const LoggerCreateInfo& createInfo) {
    Panic(createInfo.loggerMemory.data() && createInfo.loggerMemory.len() > 0,
          "Log buffer must be provided to initialize the logging system.");

    loggerMemory = createInfo.loggerMemory;

    auto tagIndicesToIgnore = createInfo.tagIndicesToIgnore;

    for (addr_size i = 0; i < tagIndicesToIgnore.len(); ++i) {
        Panic(tagIndicesToIgnore[i] < i32(MAX_NUMBER_OF_TAGS), "Trying to ignore tag index that is out of range.");
        ignoredTagIndices[addr_size(tagIndicesToIgnore[i])] = true;
    }

    if (createInfo.print != nullptr) {
        printHandler = [](const char* message) { printf(message); };
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

void setLogLevel(LogLevel level) { minimumLogLevel = level; }

LogLevel getLogLevel() { return minimumLogLevel; }

void muteLogger(bool mute) { muted = mute; }

void useLoggerANSI(bool use) { useAnsi = use; }

} // namespace core

int mmcounter = 0;
