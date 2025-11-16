#include "core_exec_ctx.h"
#include "core_types.h"
#include <core_logger.h>

#include <core_ansi_escape_codes.h>
#include <core_assert_fmt.h>
#include <core_mem.h>

#include <stdio.h>

namespace core {

namespace logdetails {

constexpr LoggerState getDefaultLoggerState() {
    return {
        nullptr,
        true,
        LogLevel::L_INFO,
        {},
        false,
        {},
        {},
        0,
        0
    };
}

// Global State
LoggerState g_state = getDefaultLoggerState();
LoggerState& getLoggerState() { return g_state; }

} // logdetails

namespace {

constexpr addr_size LOGGER_INITIAL_MEMORY_SIZE = core::CORE_KILOBYTE * 5;

} // namespace

using namespace logdetails;

LoggerCreateInfo LoggerCreateInfo::createDefault() {
    LoggerCreateInfo ret;
    ret.allocatorId = 0;
    ret.print = nullptr;
    ret.useAnsi = true;
    return ret;
}

bool loggerInit(const LoggerCreateInfo& createInfo) {
    auto& state = logdetails::g_state;

    if (createInfo.print == nullptr) {
        state.printHandler = [](StrView message) {
            [[maybe_unused]] i32 ret = printf("%.*s", i32(message.len()), message.data());
            Assert(ret >= 0, "printf failed"); // printf failed, that should never happen right?
        };
    }
    else {
        state.printHandler = createInfo.print;
    }

    state.useAnsi = createInfo.useAnsi;
    state.allocatorId = createInfo.allocatorId;

    auto& actx = core::getAllocator(createInfo.allocatorId);
    void* initialBuffer = actx.alloc(LOGGER_INITIAL_MEMORY_SIZE, sizeof(char));
    state.loggerMemory = { reinterpret_cast<char*>(initialBuffer), LOGGER_INITIAL_MEMORY_SIZE };

    state.tagTranslationTable[0][0] = 'R'; // mark as reserved.

    return true;
}

void loggerDestroy() {
    auto& state = logdetails::g_state;
    core::memoryFree(std::move(state.loggerMemory), state.allocatorId);

    // reset the state:
    state = getDefaultLoggerState();
}

bool loggerSetTag(i32 idx, core::StrView tag) {
    auto& state = logdetails::g_state;

    Panic(tag.len() < MAX_TAG_LEN, "Tag is too long.");
    Panic(idx < i32(MAX_NUMBER_OF_TAGS), "Provided Tag index is out of range.");
    Panic(state.tagTranslationTable[idx][0] == '\0', "Tag index is already taken.");

    core::memcopy(state.tagTranslationTable[idx], tag.data(), tag.len());
    state.tagTranslationTable[idx][tag.len()] = '\0';
    state.tagTranslationTableCount++;

    return true;
}

void loggerSetLevel(LogLevel level) { logdetails::g_state.minimumLogLevel = level; }

LogLevel loggerGetLevel() { return logdetails::g_state.minimumLogLevel; }

void loggerSetLevel(LogLevel level, i32 tagIdx) {
    auto& state = logdetails::g_state;

    Panic(tagIdx < i32(MAX_NUMBER_OF_TAGS), "Provided Tag index is out of range.");
    Panic(state.tagTranslationTable[tagIdx][0] != '\0', "Tag is not set.");

    state.logLevelPerTag[tagIdx] = level;
}

LogLevel loggerGetLevel(i32 tagIdx) {
    auto& state = logdetails::g_state;

    Panic(tagIdx < i32(MAX_NUMBER_OF_TAGS), "Provided Tag index is out of range.");
    Panic(state.tagTranslationTable[tagIdx][0] != '\0', "Tag is not set.");

    LogLevel logLevel = state.logLevelPerTag[tagIdx];
    return logLevel;
}

void loggerMute(bool mute) { logdetails::g_state.muted = mute; }

void loggerUseANSI(bool use) { logdetails::g_state.useAnsi = use; }

void __debug_logBytes(const void *ptr, addr_size size) {
    const u8* bytePtr = reinterpret_cast<const u8*>(ptr);
    for (addr_off i = addr_off(size) - 1; i >= 0; i--) {
        logDirectStd(" {:H} ", bytePtr[i]);
    }
}

} // namespace core
