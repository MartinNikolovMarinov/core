#include <std/stacktrace.h>

#include <utils.h>
#include <mem.h>

#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

namespace core {

std::string stacktrace(u32 nStackFrames, u32 skipFrames) {
    // TODO: print the thread I am on in the initial line next to the traceback.
    std::string result;

    // Storage array for stack trace address data:
    void** addrList = reinterpret_cast<void**>(std::malloc(nStackFrames * sizeof(void*)));
    if (addrList == nullptr) {
        result += "  <failed to allocate memory for stacktrace>\n";
        return result;
    }
    defer { std::free(addrList); };

    // Retrieve current stack addresses:
    i32 addrLen = backtrace(addrList, nStackFrames);
    if (addrLen == 0) {
        result += "  <empty, possibly corrupt>\n";
        return result;
    }

    char** symbolList = backtrace_symbols(addrList, addrLen);
    if (symbolList == nullptr) {
        result += "  <failed to backtrace symbols>\n";
        return result;
    }
    defer { std::free(symbolList); };

    constexpr u32 MAX_FUNCNAME_SIZE = 256;
    char funcName[MAX_FUNCNAME_SIZE];

    for (i32 i = skipFrames; i < addrLen; i++) {
        char *beginMangledName = 0;
        char *beginOffset = 0;
        char *endOffset = 0; // ensure that parsing was successful

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbolList[i]; *p; ++p) {
            if (*p == '(') {
                beginMangledName = p;
            }
            else if (*p == '+') {
                beginOffset = p;
            }
            else if (*p == ')' && beginOffset) {
                endOffset = p;
                break;
            }
        }

        if (beginMangledName && beginOffset && endOffset && beginMangledName < beginOffset) {
            *beginMangledName++ = '\0';
            *beginOffset++ = '\0';
            *endOffset = '\0';

            i32 status;
            u64 maxFuncNameSize = MAX_FUNCNAME_SIZE;
            char* ret = abi::__cxa_demangle(beginMangledName, funcName, &maxFuncNameSize, &status);
            if (status == 0) {
                core::memcopy(funcName, ret, maxFuncNameSize);
                result += "  ";
                result += symbolList[i];
                result += " : ";
                result += funcName;
                result += " +";
                result += beginOffset;
                result += "\n";
            }
            else {
                // Demangling failed. Output mangled function:
                result += "  ";
                result += symbolList[i];
                result += " : ";
                result += beginMangledName;
                result += " +";
                result += beginOffset;
                result += " <demangling failed: status = ";
                result += std::to_string(status);
                result += ">\n";
            }
        }
        else {
            // Couldn't parse the line, so print the whole line:
            result += "  ";
            result += symbolList[i];
            result += " <failed to parse line>";
            result += "\n";
        }
    }

    return result;
}

} // namespace core
