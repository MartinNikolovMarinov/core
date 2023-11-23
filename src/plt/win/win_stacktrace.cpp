#include <plt/core_stacktrace.h>

#include <windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

void stacktrace(char* buf, addr_size bufMax, addr_size& bufWritten,
                int nStackFrames, int skipFrames = 1) {
    void* stack[nStackFrames + skipFrames];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);

    USHORT frames = CaptureStackBackTrace(skipFrames, nStackFrames, stack, NULL);
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    defer { free(symbol); };

    bufWritten = 0;
    for (int i = 0; i < frames; ++i) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

        DWORD64 displacement = 0;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if (SymGetLineFromAddr64(process, symbol->Address, (PDWORD)&displacement, &line)) {
            // Successfully found the line
            int written = snprintf(buf + bufWritten, bufMax - bufWritten,
                                   "at %s in %s: line: %lu: address: 0x%0X\n",
                                   symbol->Name, line.FileName, line.LineNumber, symbol->Address);
            if (written > 0) {
                bufWritten += written;
            }
        } else {
            // Line information not available
            int written = snprintf(buf + bufWritten, bufMax - bufWritten,
                                   "at %s, address 0x%0X\n", symbol->Name, symbol->Address);
            if (written > 0) {
                bufWritten += written;
            }
        }
    }

    SymCleanup(process);
}
