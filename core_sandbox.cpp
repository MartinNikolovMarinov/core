#include "tests/t-index.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

void assertHandler(const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg) {
    constexpr u32 stackFramesToSkip = 2;
    constexpr addr_size stackTraceBufferSize = core::CORE_KILOBYTE * 8;
    char trace[stackTraceBufferSize] = {};
    addr_size traceLen = 0;
    bool ok = core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);

    std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                << "[ASSERTION]:\n  [EXPR]: " << failedExpr
                << "\n  [FUNC]: " << funcName
                << "\n  [FILE]: " << file << ":" << line
                << "\n  [MSG]: " << (errMsg ? errMsg : "") // IMPORTANT: MSVC's std implementation will crash if errMsg is nullptr !
                << ANSI_RESET()
                << std::endl;
    std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;

    if (!ok) {
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "Failed to take full stacktrace. Consider resizing the stacktrace buffer size!"
                  << ANSI_RESET()
                  << std::endl;
    }

    throw std::runtime_error("Assertion failed!");
};

constexpr u32 TRACE_EVENT_NAME_MAX_SIZE = 256;
constexpr u32 TRACE_EVENT_CATEGORY_MAX_SIZE = 128;

struct TraceEvent {
    char name[TRACE_EVENT_NAME_MAX_SIZE];       // Name of the event
    char category[TRACE_EVENT_CATEGORY_MAX_SIZE]; // Category of the event
    i8 ph;    // Phase: 0 -> Begin (B), 1 -> End (E)
    u64 ts;   // Timestamp (microseconds)
    u64 pid;  // Process ID
    u64 tid;  // Thread ID

    static TraceEvent create(const char* n, const char* c, i8 phase, u64 timestamp, u64 p, u64 t) {
        TraceEvent ret;
        std::snprintf(ret.name,     TRACE_EVENT_NAME_MAX_SIZE,     "%s", n);
        std::snprintf(ret.category, TRACE_EVENT_CATEGORY_MAX_SIZE, "%s", c);
        ret.ph  = phase;
        ret.ts  = timestamp;
        ret.pid = p;
        ret.tid = t;
        return ret;
    }

    // Convert this event to a JSON object string
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{ "
            << "\"name\": \"" << name << "\", "
            << "\"cat\": \""  << category << "\", "
            << "\"ph\": \""   << ((ph == 0) ? "B" : "E") << "\", "
            << "\"ts\": "     << ts << ", "
            << "\"pid\": "    << pid << ", "
            << "\"tid\": "    << tid
            << " }";
        return oss.str();
    }
};

thread_local u64 g_threadId = core::Unpack(core::threadingGetCurrentId());
u64 g_processId = core::Unpack(core::processGetId());
std::vector<TraceEvent> g_events;

#define TRACE_FUNCTION(functionName) \
    TraceEvent __start_ev = TraceEvent::create(functionName, "tracing", 0, core::getPerfCounter() / 1000000, g_processId, g_threadId); \
    g_events.push_back(__start_ev); \
    defer { \
        TraceEvent __end_ev = TraceEvent::create(functionName, "tracing", 1, core::getPerfCounter() / 1000000, g_processId, g_threadId); \
        g_events.push_back(__end_ev); \
    }

// void exampleFunction() {
//     TRACE_FUNCTION("exampleFunction");

//     // Simulate work
//     core::threadingSleep(1000);
// }

// int main() {
//     core::initProgramCtx(assertHandler);

//     // Create some example TraceEvents
//     exampleFunction();

//     // Write JSON array to a file
//     std::ofstream outFile("trace_events.json", std::ios::out);
//     if (!outFile) {
//         std::cerr << "Failed to open trace_events.json for writing\n";
//         return 1;
//     }

//     outFile << "[\n";
//     for (size_t i = 0; i < g_events.size(); ++i) {
//         outFile << "  " << g_events[i].toJson();
//         if (i + 1 < g_events.size()) {
//             outFile << ",";
//         }
//         outFile << "\n";
//     }
//     outFile << "]\n";
//     outFile.close();

//     std::cout << "Wrote trace_events.json. Load it via chrome://tracing or Perfetto UI.\n";
//     return 0;
// }

#include <x86intrin.h>
#include <sys/time.h>

u64 GetOSTimerFreq() {
    return 1000000;
}

u64 ReadOSTimer() {
    // NOTE(casey): The "struct" keyword is not necessary here when compiling in C++,
    // but just in case anyone is using this file from C, I include it.
    struct timeval Value;
    gettimeofday(&Value, 0);

    u64 Result = GetOSTimerFreq()*(u64)Value.tv_sec + (u64)Value.tv_usec;
    return Result;
}

inline u64 ReadCPUTimer(void) {
    return __rdtsc();
}

i32 main(int ArgCount, char **Args)
{
	u64 MillisecondsToWait = 1000;
	if(ArgCount == 2)
	{
		MillisecondsToWait = atol(Args[1]);
	}

	u64 OSFreq = GetOSTimerFreq();
	printf("    OS Freq: %llu (reported)\n", OSFreq);

	u64 CPUStart = ReadCPUTimer();
	u64 OSStart = ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
	while(OSElapsed < OSWaitTime)
	{
		OSEnd = ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}

	u64 CPUEnd = ReadCPUTimer();
	u64 CPUElapsed = CPUEnd - CPUStart;
	u64 CPUFreq = 0;
	if(OSElapsed)
	{
		CPUFreq = OSFreq * CPUElapsed / OSElapsed;
	}

	printf("   OS Timer: %llu -> %llu = %llu elapsed\n", OSStart, OSEnd, OSElapsed);
	printf(" OS Seconds: %.4f\n", (f64)OSElapsed/(f64)OSFreq);

	printf("  CPU Timer: %llu -> %llu = %llu elapsed\n", CPUStart, CPUEnd, CPUElapsed);
	printf("   CPU Freq: %llu (guessed)\n", CPUFreq);

    return 0;
}
