#include "tests/t-index.h"

#include <iostream>

// namespace core {
//     template <typename T> u32 hash(const T& val) = delete;
// }

// template <typename T>
// concept Squareable =
//     requires {
//         requires core::is_same_v<decltype(&T::square), i32 (T::*)(i32)> ||
//                  core::is_same_v<decltype(&T::square), f64 (T::*)(f64)>;
//     };

// template <typename T>
// concept Hashable32bit = requires(const T& a) {
//     { core::hash<T>(a) } -> core::same_as<u32>;
// };

// template <typename T>
// concept SquareHashable = Squareable<T> && Hashable32bit<T>;

// struct A {
//     i32 square(i32 x) {
//         return x * x;
//     }
// };

// namespace core {
//     template <>
//     u32 hash<A>(const A&) {
//         return 0;
//     }
// };

// struct B {
//     i64 square(i32 x) {
//         return x * x;
//     }
// };

// struct C {};

// struct D {
//     f64 square(f64 x) {
//         return x * x;
//     }
// };

// namespace core {
//     u64 hash(const D&) {
//         return 0;
//     }
// };

i32 main() {
    {
        // const char* input[] = {
        //     "name_of_program",
        //     "-bool-1 ", " t",
        //     "-bool-2   ", "T",
        //     "-bool-3  ", " true",
        //     "  -bool-4", "  TRUE",
        //     "--bool-5 ", "True  ",
        //     "    -bool-6", "1",
        //     "-bool-7", "false",
        //     "-bool-8", "zxcasdasd",

        //     " -int32", "0004",
        //     "--int64  ", "  -13",
        //     "\t-uint32", "19 ",
        //     "   -uint64", "\t99asad",

        //     "-string", "  banicata   fsa  ",

        //     "-float32-1", "  1.2",
        //     "-float32-2", "  .5. ",
        //     "-float32-3", " 1. ",
        //     "--float32-4", "  -1.2cvxc",
        //     "-float64-5", " 1.2.",
        //     "-float64-6", "7...  ",
        //     "-float64-7", "-1.2",
        //     "-float64-8", "00012.000005",
        // };

        const char* input[] = {
            "name_of_program", "arg1", "arg2",
            "-bool-1 ", " t",
            "-int32-1  ", " -4",
            "--option-1",
            " -int32-2  ", "  -4 ",
            "--option-2",
            "-trailing"
        };
        const addr_size inputSize = sizeof(input) / sizeof(input[0]);

        core::CmdFlagParser<> parser;

        auto ret = parser.parse(inputSize, input);
        Assert(!ret.hasErr());

        auto& symbols = parser.parsedSymbols();
        for (addr_size i = 0; i < symbols.len(); ++i) {
            std::cout << "symbol[" << i32(symbols[i].type) << "] "
                      << (symbols[i].value.view().data() ? symbols[i].value.view().data() : "null")
                      << std::endl;
        }

        parser.arguments([](core::StrView arg, addr_size idx) -> bool {
            std::cout << "arg[" << idx << "] " << arg.data() << std::endl;
            return true;
        });

        parser.flags([](core::StrView flag, core::StrView value) -> bool {
            std::cout << "flag[" << flag.data() << "] "
                      << (value.data() ? value.data() : "null")
                      << std::endl;
            return true;
        });

        parser.options([](core::StrView option) -> bool {
            std::cout << "option[" << option.data() << "]" << std::endl;
            return true;
        });
    }

    return 0;
}
