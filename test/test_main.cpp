// TODO: might use google test, if it can work without stdlib.

#include "../src/types.h"
#include "../src/core.h"

using namespace core;

#define run_test(test) test(#test)

void PrintTestOK(constptr char* tname) {
    PrintF("%s [OK]\n", tname);
}

#include "rune_tests.cpp"

i32 main(i32 argc, char** argv, char **envp) {
    PrintF("\n##### Running Tests #####\n\n");

    if (run_test(RunAllRuneTests) != true) {
        Panic("string tests failed");
    }

    return 0;
}
