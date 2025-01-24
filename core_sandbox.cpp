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
    i32 pid;  // Process ID
    i32 tid;  // Thread ID

    static TraceEvent create(const char* n, const char* c, i8 phase, u64 timestamp, i32 p, i32 t) {
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

int main() {
    core::initProgramCtx(assertHandler);

    // Create some example TraceEvents
    TraceEvent event1 = TraceEvent::create("Task A", "Category1", 0, 1000, 1, 101);
    TraceEvent event2 = TraceEvent::create("Task A", "Category1", 1, 2000, 1, 101);
    TraceEvent event3 = TraceEvent::create("Task B", "Category2", 0, 1500, 1, 102);
    TraceEvent event4 = TraceEvent::create("Task B", "Category2", 1, 2500, 1, 102);

    // Collect them in a vector
    std::vector<TraceEvent> events = {event1, event2, event3, event4};

    // Write JSON array to a file
    std::ofstream outFile("trace_events.json", std::ios::out);
    if (!outFile) {
        std::cerr << "Failed to open trace_events.json for writing\n";
        return 1;
    }

    outFile << "[\n";
    for (size_t i = 0; i < events.size(); ++i) {
        outFile << "  " << events[i].toJson();
        if (i + 1 < events.size()) {
            outFile << ",";
        }
        outFile << "\n";
    }
    outFile << "]\n";
    outFile.close();

    std::cout << "Wrote trace_events.json. Load it via chrome://tracing or Perfetto UI.\n";
    return 0;
}
