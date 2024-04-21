#include "tests/t-index.h"

#include <iostream>
#include <unordered_map>
#include <ctime>
#include <cstdlib>
#include <string>

i32 main() {
    core::initProgramCtx(nullptr, nullptr);

    core::HashMap<i32, i32> map;
    i32 b = 0;
    map.put(7, b);
    map.put(b, 7);
    map.put(7, 7);
    map.put(b, b);
    i32* v = map.get(7);
    if (v == nullptr) {
        return -1;
    }
    return *v;
}
