#include "t-plt.cpp"
#include "t-core_error.cpp"

void RunAllSTDTests() {
    RunTestSuite(RunPltTestSuite);
    RunTestSuite(CoreErrorTestSute);
}
