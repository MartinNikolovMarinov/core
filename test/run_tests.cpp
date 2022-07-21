#include "t-bits.cpp"
#include "t-rnd.cpp"
#include "t-rune.cpp"

void RunAllTests() {
    RunTestSuite(RunBitsTestsSuite);
    RunTestSuite(RunRndTestsSuite);
    RunTestSuite(RunRuneTestsSuite);
}
