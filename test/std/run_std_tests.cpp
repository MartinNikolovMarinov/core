#include "t-plt.cpp"
#include "t-core_error.cpp"
#include "t-vector.cpp"
#include "t-alloc.cpp"

void RunAllSTDTests() {
    RunTestSuite(RunPltTestSuite);
    RunTestSuite(CoreErrorTestSute);
    RunTestSuite(VectorTestSuite);
    RunTestSuite(AllocTestSuite);
}
