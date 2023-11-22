#include <plt/core_stacktrace.h>

#include <core_utils.h>
#include <core_mem.h>
#include <core_cptr.h>
#include <core_cptr_conv.h>

#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

namespace core {

void stacktrace(char* buf, addr_size bufMax, addr_size& bufWritten,
                i32 nStackFrames, i32 skipFrames) {

    // TODO: print the thread I am on in the initial line next to the traceback.

    auto writeToBuff = [&](const char* s) -> bool {
        auto slen = core::cptrLen(s);
        if (bufWritten + slen >= bufMax) {
            return false;
        }
        bufWritten += slen;
        buf = core::cptrCopy(buf, s, slen);
        return true;
    };

    // Storage array for stack trace address data:
    void** addrList = reinterpret_cast<void**>(std::malloc(size_t(nStackFrames) * sizeof(void*)));
    if (addrList == nullptr) {
        writeToBuff("  <failed to allocate memory for stacktrace>\n");
        return;
    }
    defer { std::free(addrList); };

    // Retrieve current stack addresses:
    i32 addrLen = backtrace(addrList, nStackFrames);
    if (addrLen == 0) {
        writeToBuff("  <empty, possibly corrupt>\n");
        return;
    }

    char** symbolList = backtrace_symbols(addrList, addrLen);
    if (symbolList == nullptr) {
        writeToBuff("  <failed to backtrace symbols>\n");
        return;
    }
    defer { std::free(symbolList); };

    constexpr u32 MAX_FUNCNAME_SIZE = 1024;
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
            *beginMangledName++ = core::term_char;
            *beginOffset++ = core::term_char;
            *endOffset = core::term_char;

            i32 status;
            unsigned long maxFuncNameSize = MAX_FUNCNAME_SIZE; // NOTE: unsigned long because of MAC ...
            char* ret = abi::__cxa_demangle(beginMangledName, funcName, &maxFuncNameSize, &status);
            if (status == 0) {
                core::memcopy(funcName, ret, maxFuncNameSize);
                writeToBuff("  ");
                writeToBuff(symbolList[i]);
                writeToBuff(" : ");
                writeToBuff(funcName);
                writeToBuff(" +");
                writeToBuff(beginOffset);
                writeToBuff("\n");
            }
            else {
                // Demangling failed. Output mangled function:
                writeToBuff("  ");
                writeToBuff(symbolList[i]);
                writeToBuff(" : ");
                writeToBuff(beginMangledName);
                writeToBuff(" +");
                writeToBuff(beginOffset);
                writeToBuff(" <demangling failed: status = ");
                {
                    char strStatus[20];
                    core::intToCptr(status, strStatus, 20);
                    writeToBuff(strStatus);
                }
                writeToBuff(">\n");
            }
        }
        else {
            // Couldn't parse the line, so print the whole line:
            writeToBuff("  ");
            writeToBuff(symbolList[i]);
            writeToBuff(" <failed to parse line>\n");
            writeToBuff("\n");
        }
    }
}

} // namespace core
