#include <plt/core_stacktrace.h>
#include <plt/core_threading.h>

#include <core_cstr_conv.h>

#include <cstdlib>
#include <execinfo.h>
#include <cxxabi.h>

namespace core {

bool stacktrace(char* buf, addr_size bufMax, addr_size& bufWritten, i32 nStackFrames, i32 skipFrames) {
    auto writeToBuf = [&](const char* s) -> bool {
        auto slen = core::cstrLen(s);
        if (bufWritten + slen >= bufMax) {
            return false;
        }
        bufWritten += slen;
        buf = core::memcopy(buf, s, slen);
        return true;
    };

    bufWritten = 0;

    // Write the thread name
    {
        char threadingBuffer[MAX_THREAD_NAME_LENGTH] = {};
        if (!core::threadingGetName(threadingBuffer).hasErr()) {
            if (!writeToBuf("Thread: ")) return false;
            if (!writeToBuf(threadingBuffer[0] ? threadingBuffer : "unnamed")) return false;
            if (!writeToBuf("\n")) return false;
        }
        else {
            if (!writeToBuf("Thread: <failed to get name>\n")) return false;
        }
    }

    // Capture the backtrace
    void** callstack = reinterpret_cast<void**>(std::malloc(addr_size(nStackFrames + skipFrames) * sizeof(void*)));
    if (callstack == nullptr) {
        writeToBuf("  <failed to allocate callstack>\n");
        return false;
    }
    i32 framesCount = backtrace(callstack, nStackFrames + skipFrames - 1);
    if (framesCount == 0) {
        writeToBuf("  <empty, possibly corrupt>\n");
        return false;
    }
    defer {
        std::free(callstack);
    };

    // Get the symbols
    char** symbols = backtrace_symbols(callstack, framesCount);
    if (symbols == nullptr) {
        writeToBuf("  <failed to backtrace symbols>\n");
        return false;
    }
    defer { std::free(symbols); };

    // Write the stack frames
    for (i32 i = skipFrames; i < framesCount; i++) {
        char* symbol = symbols[i];

        char* beginName = nullptr;
        char* beginOffset = nullptr;
        char* endOffset = nullptr;

        // Find the parentheses and +address offset surrounding the mangled name
        for (char* p = symbols[i]; *p; ++p) {
            if (*p == '(') {
                beginName = p;
            }
            else if (*p == '+') {
                beginOffset = p;
            }
            else if (*p == ')' && beginOffset) {
                endOffset = p;
                break;
            }
        }

        bool successfullyParsedLine = beginName && beginOffset && endOffset && beginName < beginOffset;

        if (successfullyParsedLine) {
            *beginName++ = core::term_char;
            *beginOffset++ = core::term_char;
            *endOffset = core::term_char;

            i32 status = 0;
            char* demangled = abi::__cxa_demangle(beginName, nullptr, nullptr, &status);
            defer { if (demangled) std::free(demangled); };

            bool demangleSuccess = (status == 0);

            if (demangleSuccess) {
                if (!writeToBuf("  "))        return false;
                if (!writeToBuf(symbol))      return false;
                if (!writeToBuf(" : "))       return false;
                if (!writeToBuf(demangled))   return false;
                if (!writeToBuf(" +"))        return false;
                if (!writeToBuf(beginOffset)) return false;
                if (!writeToBuf("\n"))        return false;
            }
            else {
                // Demangling failed. Output mangled function:
                if (!writeToBuf("  "))                             return false;
                if (!writeToBuf(symbol))                           return false;
                if (!writeToBuf(" : "))                            return false;
                if (!writeToBuf(beginName))                        return false;
                if (!writeToBuf(" +"))                             return false;
                if (!writeToBuf(beginOffset))                      return false;
                if (!writeToBuf(" <demangling failed: status = ")) return false;
                {
                    constexpr addr_size strStatusLen = 20;
                    char strStatus[strStatusLen] = {};
                    core::intToCstr(status, strStatus, strStatusLen);
                    if (!writeToBuf(strStatus)) return false;
                }
                if (!writeToBuf(">\n")) return false;
            }
        }
        else {
            if (!writeToBuf("  "))                        return false;
            if (!writeToBuf(symbol))                      return false;
            if (!writeToBuf(" <failed to parse line>\n")) return false;
            if (!writeToBuf("\n"))                        return false;
        }
    }

    return true;
}

} // namespace core
