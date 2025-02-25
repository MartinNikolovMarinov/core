#include <core_logger.h>

#include <core_ansi_escape_codes.h>
#include <core_mem.h>

#include <stdio.h>

namespace core {

namespace logdetails {

// Global State
LoggerState g_state = {
    nullptr,
    true,
    LogLevel::L_INFO,
    {},
    false,
    {},
    {},
    0
};
LoggerState& getLoggerState() { return g_state; }

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

    auto& state = logdetails::g_state;
    state.loggerMemory = createInfo.loggerMemory;
    auto tagIndicesToIgnore = createInfo.tagIndicesToIgnore;

    for (addr_size i = 0; i < tagIndicesToIgnore.len(); ++i) {
        Panic(tagIndicesToIgnore[i] < i32(MAX_NUMBER_OF_TAGS), "Trying to ignore tag index that is out of range.");
        state.ignoredTagIndices[addr_size(tagIndicesToIgnore[i])] = true;
    }

    if (createInfo.print == nullptr) {
        state.printHandler = [](const char* message, i32 messageLen) {
            [[maybe_unused]] i32 ret = printf("%.*s", messageLen, message);
            Assert(ret >= 0, "printf failed"); // printf failed, that should never happen right?
        };
    }
    else {
        state.printHandler = createInfo.print;
    }

    state.useAnsi = createInfo.useAnsi;

    return true;
}

bool setLoggerTag(i32 idx, core::StrView tag) {
    auto& state = logdetails::g_state;

    Panic(tag.len() < MAX_TAG_LEN, "Tag is too long.");
    Panic(idx < i32(MAX_NUMBER_OF_TAGS), "Provided Tag index is out of range.");
    Panic(state.tagTranslationTable[idx][0] == '\0', "Tag index is already taken.");

    core::memcopy(state.tagTranslationTable[idx], tag.data(), tag.len());
    state.tagTranslationTableCount++;

    return true;
}

void setLogLevel(LogLevel level) { logdetails::g_state.minimumLogLevel = level; }

LogLevel getLogLevel() { return logdetails::g_state.minimumLogLevel; }

void muteLogger(bool mute) { logdetails::g_state.muted = mute; }

void useLoggerANSI(bool use) { logdetails::g_state.useAnsi = use; }

void __debug_logBytes(const void *ptr, addr_size size) {
    const u8* bytePtr = reinterpret_cast<const u8*>(ptr);
    for (addr_off i = addr_off(size) - 1; i >= 0; i--) {
        logDirectStd(" {:H} ", bytePtr[i]);
    }
}

} // namespace core
