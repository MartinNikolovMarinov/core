#include "tests/t-index.h"

#include <iostream>
#include <unordered_map>
#include <ctime>
#include <cstdlib>
#include <string>

i32 main() {
    constexpr i32 NUM_SAMPLES = 1000000;
    u32 seed = 0;
    std::unordered_map<u64, u64> histogram;

    srand(u32(time(NULL)));

    for (i32 i = 0; i < NUM_SAMPLES; i++) {
        auto rnd = rand();
        std::string testStr = std::to_string(rnd);

        u64 hash = core::djb2_64(testStr.c_str(), testStr.size(), seed);

        histogram[hash]++;
    }

    std::cout << "Collisions: " << NUM_SAMPLES - histogram.size() << std::endl;

    return 0;
}
