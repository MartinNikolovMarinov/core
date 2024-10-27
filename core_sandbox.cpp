#include "tests/t-index.h"

#include <iomanip>
#include <iostream>

constexpr const char* digits = "01.126";
constexpr addr_size digitsSize = core::cstrLen(digits);
constexpr auto res = core::cstrToFloat<f32>(digits, digitsSize);

i32 main() {
    if (res.hasErr()) {
        std::cout << core::parseErrorToCstr(res.err()) << std::endl;
        return 0;
    }
    std::cout << res.value() << std::endl;
    return 0;
}
