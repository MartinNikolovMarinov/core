#include <core.h>

#include <iostream>

using namespace coretypes;

bool GlobalAssertHandler(
    const char* failedExpr,
    const char* file, i32 line,
    const char* errMsg
) {
    std::cout << "[ASSERTION] [EXPR]: " << failedExpr
              << " [FILE]: " << file
              << " [LINE]: " << line
              << " [MSG]: " << errMsg
              << std::endl; // flush stream!
    return true;
}

i32 main(i32 argc, const char *argv[]) {
    core::SetGlobalAssertHandler(&GlobalAssertHandler);
    Assert(false);
    return 0;
}
