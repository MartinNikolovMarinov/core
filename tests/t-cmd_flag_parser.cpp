#include "t-index.h"

const char* __cmd_flag_test_input_1[] = {
    "das.exe", "adsad", "zxczxc",
    "-b", "asd",
    "-basd", "-123",
    "--bb",
    "-cc", "asdasd",
    "--bb",
    "--zz"
};
constexpr addr_size __cmd_flag_test_input_1_len = sizeof(__cmd_flag_test_input_1) / sizeof(__cmd_flag_test_input_1[0]);

const char* __cmd_flag_test_input_2[] = {
    "name_of_program",
    "-bool-1 ", " t",
    "-bool-2   ", "T",
    "-bool-3  ", " true",
    "  -bool-4", "  TRUE",
    " -bool-5  ", "True  ",
    "    -bool-6", "1",
    "-bool-7", "false",
    "-bool-8", "zxcasdasd",

    " -int32", "0004",
    "-int64  ", "  -13",
    "\t-uint32", "19 ",
    "   -uint64", "\t99asad",

    "-string", "  banicata   fsa  ",

    "-float32-1", "  1.2",
    "-float32-2", "  .5. ",
    "-float32-3", " 1. ",
    "-float32-4", "  -1.2cvxc",
    "-float64-5", " 1.2.",
    "-float64-6", "7...  ",
    "-float64-7", "-1.2",
    "-float64-8", "00012.000005",
};
constexpr addr_size __cmd_flag_test_input_2_len = sizeof(__cmd_flag_test_input_2) / sizeof(__cmd_flag_test_input_2[0]);

i32 cmdFlagParserSymbolParsingTest() {
    using CmdFlagParser = core::CmdFlagParser;
    using core::sv;

    CmdFlagParser parser;

    auto ret = parser.parse(__cmd_flag_test_input_1_len, __cmd_flag_test_input_1);
    CT_CHECK(!ret.hasErr());

    auto& symbols = parser.parsedSymbols();

    CT_CHECK(symbols.len() == __cmd_flag_test_input_1_len);

    // Check the program name:
    {
        CT_CHECK(symbols[0].type == CmdFlagParser::ParsedSymbolType::ProgramName);
        CT_CHECK(symbols[0].value.eq(sv("das.exe")));
        CT_CHECK(parser.programName().eq(sv("das.exe")));
    }

    // Check the program argument list:
    {
        CT_CHECK(symbols[1].type == CmdFlagParser::ParsedSymbolType::Argument);
        CT_CHECK(symbols[1].value.eq(sv("adsad")));
        CT_CHECK(symbols[2].type == CmdFlagParser::ParsedSymbolType::Argument);
        CT_CHECK(symbols[2].value.eq(sv("zxczxc")));

        parser.arguments([](core::StrView arg, addr_size idx) -> bool {
            switch (idx) {
                case 0: CT_CHECK(arg.eq(sv("adsad"))); break;
                case 1: CT_CHECK(arg.eq(sv("zxczxc"))); break;
                default: CT_CHECK(false); break;
            }
            return true;
        });
    }

    // Check the rest:
    {
        CT_CHECK(symbols[3].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[3].value.eq(sv("b")));
        CT_CHECK(symbols[4].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[4].value.eq(sv("asd")));
        CT_CHECK(symbols[5].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[5].value.eq(sv("basd")));
        CT_CHECK(symbols[6].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[6].value.eq(sv("-123")));
        CT_CHECK(symbols[7].type == CmdFlagParser::ParsedSymbolType::OptionName);
        CT_CHECK(symbols[7].value.eq(sv("bb")));
        CT_CHECK(symbols[8].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[8].value.eq(sv("cc")));
        CT_CHECK(symbols[9].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[9].value.eq(sv("asdasd")));
        CT_CHECK(symbols[10].type == CmdFlagParser::ParsedSymbolType::OptionName);
        CT_CHECK(symbols[10].value.eq(sv("bb")));
        CT_CHECK(symbols[11].type == CmdFlagParser::ParsedSymbolType::OptionName);
        CT_CHECK(symbols[11].value.eq(sv("zz")));

        // Check the flags:
        {
            i32 flagsChecker[] = { 0, 0, 0 };
            parser.flags([&flagsChecker](core::StrView flag, core::StrView value) -> bool {
                if (flag.eq(sv("b"))) {
                    CT_CHECK(value.eq(sv("asd")));
                    flagsChecker[0]++;
                }
                else if (flag.eq(sv("basd"))) {
                    CT_CHECK(value.eq(sv("-123")));
                    flagsChecker[1]++;
                }
                else if (flag.eq(sv("cc"))) {
                    CT_CHECK(value.eq(sv("asdasd")));
                    flagsChecker[2]++;
                }
                else {
                    CT_CHECK(false, "Parsed somthing that shouldn't have been parsed!");
                }

                return true;
            });

            // Check that all flags were parsed and iterated over exactly once:
            for (auto v : flagsChecker) {
                CT_CHECK(v == 1);
            }
        }

        // Check the options:
        {
            i32 optionsChecker[] = { 0, 0 };
            parser.options([&optionsChecker](core::StrView option) -> bool {
                if (option.eq(sv("bb"))) {
                    optionsChecker[0]++;
                }
                else if (option.eq(sv("zz"))) {
                    optionsChecker[1]++;
                }
                else {
                    CT_CHECK(false, "Parsed somthing that shouldn't have been parsed!");
                }

                return true;
            });

            // Check that all options were parsed and iterated over exactly once:
            CT_CHECK(optionsChecker[0] == 2); // this should be set twice.
            CT_CHECK(optionsChecker[1] == 1);
        }
    }

    return 0;
}

i32 cmdFlagParserSymbolParsingLongerTest() {
    using CmdFlagParser = core::CmdFlagParser;
    using core::sv;

    CmdFlagParser parser;

    auto ret = parser.parse(__cmd_flag_test_input_2_len, __cmd_flag_test_input_2);
    CT_CHECK(!ret.hasErr());

    auto& symbols = parser.parsedSymbols();

    CT_CHECK(symbols.len() == __cmd_flag_test_input_2_len);

    // Check the program name:
    {
        CT_CHECK(symbols[0].type == CmdFlagParser::ParsedSymbolType::ProgramName);
        CT_CHECK(symbols[0].value.eq(sv("name_of_program")));
        CT_CHECK(parser.programName().eq(sv("name_of_program")));
    }

    // Check argument list:
    {
        i32 none = 0;
        parser.arguments([&none](core::StrView, addr_size) -> bool {
            none++;
            return true;
        });
        CT_CHECK(none == 0);
    }

    // Check the flags:
    {
        CT_CHECK(symbols[1].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[1].value.eq(sv("bool-1")));
        CT_CHECK(symbols[2].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[2].value.eq(sv("t")));
        CT_CHECK(symbols[3].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[3].value.eq(sv("bool-2")));
        CT_CHECK(symbols[4].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[4].value.eq(sv("T")));
        CT_CHECK(symbols[5].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[5].value.eq(sv("bool-3")));
        CT_CHECK(symbols[6].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[6].value.eq(sv("true")));
        CT_CHECK(symbols[7].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[7].value.eq(sv("bool-4")));
        CT_CHECK(symbols[8].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[8].value.eq(sv("TRUE")));
        CT_CHECK(symbols[9].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[9].value.eq(sv("bool-5")));
        CT_CHECK(symbols[10].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[10].value.eq(sv("True")));
        CT_CHECK(symbols[11].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[11].value.eq(sv("bool-6")));
        CT_CHECK(symbols[12].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[12].value.eq(sv("1")));
        CT_CHECK(symbols[13].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[13].value.eq(sv("bool-7")));
        CT_CHECK(symbols[14].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[14].value.eq(sv("false")));
        CT_CHECK(symbols[15].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[15].value.eq(sv("bool-8")));
        CT_CHECK(symbols[16].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[16].value.eq(sv("zxcasdasd")));
        CT_CHECK(symbols[17].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[17].value.eq(sv("int32")));
        CT_CHECK(symbols[18].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[18].value.eq(sv("0004")));
        CT_CHECK(symbols[19].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[19].value.eq(sv("int64")));
        CT_CHECK(symbols[20].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[20].value.eq(sv("-13")));
        CT_CHECK(symbols[21].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[21].value.eq(sv("uint32")));
        CT_CHECK(symbols[22].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[22].value.eq(sv("19")));
        CT_CHECK(symbols[23].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[23].value.eq(sv("uint64")));
        CT_CHECK(symbols[24].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[24].value.eq(sv("99asad")));
        CT_CHECK(symbols[25].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[25].value.eq(sv("string")));
        CT_CHECK(symbols[26].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[26].value.eq(sv("banicata   fsa")));
        CT_CHECK(symbols[27].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[27].value.eq(sv("float32-1")));
        CT_CHECK(symbols[28].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[28].value.eq(sv("1.2")));
        CT_CHECK(symbols[29].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[29].value.eq(sv("float32-2")));
        CT_CHECK(symbols[30].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[30].value.eq(sv(".5.")));
        CT_CHECK(symbols[31].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[31].value.eq(sv("float32-3")));
        CT_CHECK(symbols[32].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[32].value.eq(sv("1.")));
        CT_CHECK(symbols[33].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[33].value.eq(sv("float32-4")));
        CT_CHECK(symbols[34].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[34].value.eq(sv("-1.2cvxc")));
        CT_CHECK(symbols[35].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[35].value.eq(sv("float64-5")));
        CT_CHECK(symbols[36].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[36].value.eq(sv("1.2.")));
        CT_CHECK(symbols[37].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[37].value.eq(sv("float64-6")));
        CT_CHECK(symbols[38].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[38].value.eq(sv("7...")));
        CT_CHECK(symbols[39].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[39].value.eq(sv("float64-7")));
        CT_CHECK(symbols[40].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[40].value.eq(sv("-1.2")));
        CT_CHECK(symbols[41].type == CmdFlagParser::ParsedSymbolType::FlagName);
        CT_CHECK(symbols[41].value.eq(sv("float64-8")));
        CT_CHECK(symbols[42].type == CmdFlagParser::ParsedSymbolType::FlagValue);
        CT_CHECK(symbols[42].value.eq(sv("00012.000005")));
    }

    return 0;
}

i32 cmdFlagParserBasicErrorsTest() {
    using CmdFlagParser = core::CmdFlagParser;

    CmdFlagParser parser;

    {
        const char* input[] = { "lies" };
        auto ret = parser.parse(CmdFlagParser::MAX_ARG_COUNT + 1, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::ArgumentListTooLong);
    }
    {
        auto ret = parser.parse(5, nullptr);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::NothingToParse);
    }
    {
        const char* input[] = { "lies" };
        auto ret = parser.parse(0, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::NothingToParse);
    }
    {
        const char* input[] = { nullptr };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        auto ret = parser.parse(len, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::InvalidArgvElement);
    }
    {
        const char* input[] = { "a", "asd", nullptr };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        auto ret = parser.parse(len, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::InvalidArgvElement);
    }
    {
        const char* input[] = { "a", "-a", nullptr };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        auto ret = parser.parse(len, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::InvalidArgvElement);
    }
    {
        // Values must be associated with flags not options.
        const char* input[] = { "a", "--b", "c" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        auto ret = parser.parse(len, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::IncorrectValue);
    }
    {
        // Multiple values for a single flag are not allowed.
        const char* input[] = { "a", "-b", "c", "d", "--f" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        auto ret = parser.parse(len, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::IncorrectValue);
    }
    {
        // Dangling flag.
        const char* input[] = { "a", "-b", "c", "-d" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        auto ret = parser.parse(len, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::FlagWithoutValue);
    }
    {
        // Dangling flag.
        const char* input[] = { "a", "-b" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        auto ret = parser.parse(len, input);
        CT_CHECK(ret.hasErr());
        CT_CHECK(ret.err() == CmdFlagParser::ParseError::FlagWithoutValue);
    }

    return 0;
}

i32 cmdFlagParserDoubleParsingTest() {
    using CmdFlagParser = core::CmdFlagParser;

    CmdFlagParser parser;

    {
        const char* input[] = { "a", "-b", "c" };
        auto res = parser.parse(3, input);
        CT_CHECK(!res.hasErr());
        CT_CHECK(parser.parsedSymbols().len() == 3);
    }
    {
        const char* input[] = { "a", "-b", "c", "--f", "-d", "e" };
        auto res = parser.parse(6, input);
        CT_CHECK(!res.hasErr());
        CT_CHECK(parser.parsedSymbols().len() == 6);
    }

    return 0;
}

i32 cmdFlagParserFriendlyInputMatchingTest() {
    using CmdFlagParser = core::CmdFlagParser;
    using core::sv;

    CmdFlagParser parser;

    i32 a = 0;
    i64 b = 0;
    u32 c = 0;
    u64 d = 0;

    parser.setFlagInt32(&a, sv("int32"));
    parser.setFlagInt64(&b, sv("int64"));
    parser.setFlagUint32(&c, sv("uint32"));
    parser.setFlagUint64(&d, sv("uint64"));

    core::StrBuilder sbArg;
    parser.setFlagString(&sbArg, sv("string"));

    bool bool_1 = false;
    bool bool_2 = false;
    bool bool_3 = false;
    bool bool_4 = false;
    bool bool_5 = false;
    bool bool_6 = false;
    bool bool_7 = false;
    bool bool_8 = false;
    parser.setFlagBool(&bool_1, sv("bool-1"));
    parser.setFlagBool(&bool_2, sv("bool-2"));
    parser.setFlagBool(&bool_3, sv("bool-3"));
    parser.setFlagBool(&bool_4, sv("bool-4"));
    parser.setFlagBool(&bool_5, sv("bool-5"));
    parser.setFlagBool(&bool_6, sv("bool-6"));
    parser.setFlagBool(&bool_7, sv("bool-7"));
    parser.setFlagBool(&bool_8, sv("bool-8"));

    f32 fa = 0, fb = 0, fc = 0, fd = 0;
    f64 fe = 0, ff = 0, fg = 0, fh = 0;
    parser.setFlagFloat32(&fa, sv("float32-1"));
    parser.setFlagFloat32(&fb, sv("float32-2"));
    parser.setFlagFloat32(&fc, sv("float32-3"));
    parser.setFlagFloat32(&fd, sv("float32-4"));
    parser.setFlagFloat64(&fe, sv("float64-5"));
    parser.setFlagFloat64(&ff, sv("float64-6"));
    parser.setFlagFloat64(&fg, sv("float64-7"));
    parser.setFlagFloat64(&fh, sv("float64-8"));

    {
        auto ret = parser.parse(__cmd_flag_test_input_2_len, __cmd_flag_test_input_2);
        CT_CHECK(!ret.hasErr());
    }

    {
        auto ret = parser.matchFlags();
        CT_CHECK(!ret.hasErr());
    }

    // Check integer parsing:
    {
        CT_CHECK(a == 4);
        CT_CHECK(b == -13);
        CT_CHECK(c == 19);
        CT_CHECK(d == 99);
    }

    // Check string parsing:
    {
        CT_CHECK(!sbArg.empty());
        CT_CHECK(core::cptrCmp(sbArg.view().data(), sbArg.view().len(), "banicata   fsa", core::cptrLen("banicata   fsa")) == 0);
    }

    // Check boolean parsing:
    {
        CT_CHECK(bool_1);
        CT_CHECK(bool_2);
        CT_CHECK(bool_3);
        CT_CHECK(bool_4);
        CT_CHECK(bool_5);
        CT_CHECK(bool_6);
        CT_CHECK(!bool_7);
        CT_CHECK(!bool_8);
    }

    // Check float parsing:
    {
        CT_CHECK(core::safeEq(fa, 1.2f, 0.00001f));
        CT_CHECK(core::safeEq(fb, 0.5f, 0.00001f));
        CT_CHECK(core::safeEq(fc, 1.0f, 0.00001f));
        CT_CHECK(core::safeEq(fd, -1.2f, 0.00001f));
        CT_CHECK(core::safeEq(fe, 1.2, 0.00001));
        CT_CHECK(core::safeEq(ff, 7.0, 0.00001));
        CT_CHECK(core::safeEq(fg, -1.2, 0.00001));
        CT_CHECK(core::safeEq(fh, 12.000005, 0.00001));

    }

    return 0;
}

i32 cmdFlagParserMatchingEdgecasesTest() {
    using CmdFlagParser = core::CmdFlagParser;
    using core::sv;

    {
        CmdFlagParser parser;
        i32 a = 0;

        parser.setFlagInt32(&a, sv("int32"));

        {
            const char* input[] = { "exe", "-int32 5", "8" };
            constexpr addr_size len = sizeof(input) / sizeof(input[0]);
            auto ret = parser.parse(len, input);
            CT_CHECK(!ret.hasErr());
        }
        {
            auto ret = parser.matchFlags();
            CT_CHECK(ret.hasErr());
            CT_CHECK(ret.err() == CmdFlagParser::MatchError::UnknownFlag);
        }
    }

    {
        CmdFlagParser parser;
        i32 a = 0;

        {
            const char* input[] = { "exe", "-int32", "8" };
            constexpr addr_size len = sizeof(input) / sizeof(input[0]);
            auto ret = parser.parse(len, input);
            CT_CHECK(!ret.hasErr());
        }
        {
            auto ret = parser.matchFlags();
            CT_CHECK(ret.hasErr());
            CT_CHECK(ret.err() == CmdFlagParser::MatchError::UnknownFlag);
        }
        parser.setFlagInt32(&a, sv("int32"));
        {
            // Match again after setting the flag.
            auto ret = parser.matchFlags();
            CT_CHECK(!ret.hasErr());
        }
    }

    // Test required flag predicated:
    {
        CmdFlagParser parser;
        i32 a;
        i64 b;

        parser.setFlagInt32(&a, sv("int32"), true);
        parser.setFlagInt64(&b, sv("int64"), false);

        {
            const char* input[] = { "exe", "-int64", "8" };
            constexpr addr_size len = sizeof(input) / sizeof(input[0]);
            auto ret = parser.parse(len, input);
            CT_CHECK(!ret.hasErr());
        }
        {
            auto ret = parser.matchFlags();
            CT_CHECK(ret.hasErr());
            CT_CHECK(ret.err() == CmdFlagParser::MatchError::MissingRequiredFlag);
        }
        {
            const char* input[] = { "exe", "-int32", "8" };
            constexpr addr_size len = sizeof(input) / sizeof(input[0]);
            auto ret = parser.parse(len, input);
            CT_CHECK(!ret.hasErr());
        }
        {
            auto ret = parser.matchFlags();
            CT_CHECK(!ret.hasErr());
        }
    }

    // Test allow unknown flags
    {
        CmdFlagParser parser;

        {
            const char* input[] = { "exe", "-int32", "8", "-int64", "8" };
            constexpr addr_size len = sizeof(input) / sizeof(input[0]);
            auto ret = parser.parse(len, input);
            CT_CHECK(!ret.hasErr());
        }
        {
            auto ret = parser.matchFlags();
            CT_CHECK(ret.hasErr());
            CT_CHECK(ret.err() == CmdFlagParser::MatchError::UnknownFlag);
        }
        parser.allowUnknownFlags(true);
        {
            auto ret = parser.matchFlags();
            CT_CHECK(!ret.hasErr());
        }
    }

    return 0;
}

i32 cmdParserValidationRulesTest() {
    using CmdFlagParser = core::CmdFlagParser;
    using core::sv;

    CmdFlagParser parser;
    i32 a, b;

    // Add some rules
    parser.setFlagInt32(&a, sv("a"), false, [](void* _a) -> bool {
        i32 v = *reinterpret_cast<i32*>(_a);
        return (v > 0);
    });
    parser.setFlagInt32(&b, sv("b"), false, [](void* _b) -> bool {
        i32 v = *reinterpret_cast<i32*>(_b);
        return (v <= 10);
    });

    // Check if the rules catch errors, when they should.

    {
        const char* input[] = { "exe", "-a", "1", "-b", "5" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        Expect(parser.matchFlags());
    }
    {
        const char* input[] = { "exe", "-a", "0", "-b", "5" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        auto res = parser.matchFlags();
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == CmdFlagParser::MatchError::ValidationFailed);
    }
    {
        const char* input[] = { "exe", "-a", "1", "-b", "10" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        Expect(parser.matchFlags());
    }
    {
        const char* input[] = { "exe", "-a", "0", "-b", "11" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        auto res = parser.matchFlags();
        CT_CHECK(res.hasErr());
        CT_CHECK(res.err() == CmdFlagParser::MatchError::ValidationFailed);
    }
    {
        const char* input[] = { "exe", "-a", "1", "-b", "10" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        Expect(parser.matchFlags());
    }
    {
        const char* input[] = { "bin", "-a", "1" }; // b is not required, the custom validator should not fail this case!
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        Expect(parser.matchFlags());
    }

    return 0;
}

i32 cmdParserAliasTest() {
    using CmdFlagParser = core::CmdFlagParser;
    using core::sv;

    // Basic aliasing:
    {
        CmdFlagParser parser;
        i32 arg1 = -1;
        bool arg2 = false;

        parser.setFlagInt32(&arg1, core::sv("full_name"));
        parser.setFlagBool(&arg2, core::sv("full_name_2"));
        parser.alias(core::sv("full_name"), core::sv("a"));
        parser.alias(core::sv("full_name_2"), core::sv("b"));

        const char* input[] = { "bin", "-a", "1", "-b", "true" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        Expect(parser.matchFlags());

        CT_CHECK(arg1 == 1);
        CT_CHECK(arg2 == true);
    }

    // Multiple aliases to the same flag:
    {
        CmdFlagParser parser;
        core::StrBuilder doubleAliased;

        parser.setFlagString(&doubleAliased, core::sv("full_name"));

        parser.alias(core::sv("full_name"), core::sv("a"));
        parser.alias(core::sv("full_name"), core::sv("b"));

        const char* input[] = { "bin", "-a", "value", "-b", "override" };
        constexpr addr_size len = sizeof(input) / sizeof(input[0]);
        Expect(parser.parse(len, input));
        Expect(parser.matchFlags());

        CT_CHECK(core::cptrCmp(doubleAliased.view().data(), doubleAliased.view().len(), "override", core::cptrLen("override")) == 0);
    }

    return 0;
}

i32 runCmdParserTestsSuite() {
    using namespace core::testing;

    auto runTests = [] (TestInfo& tInfo, const char* description, i32& retCode) {
        tInfo.description = description;

        tInfo.name = FN_NAME_TO_CPTR(cmdFlagParserSymbolParsingTest);
        if (runTest(tInfo, cmdFlagParserSymbolParsingTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(cmdFlagParserSymbolParsingLongerTest);
        if (runTest(tInfo, cmdFlagParserSymbolParsingLongerTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(cmdFlagParserBasicErrorsTest);
        if (runTest(tInfo, cmdFlagParserBasicErrorsTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(cmdFlagParserDoubleParsingTest);
        if (runTest(tInfo, cmdFlagParserDoubleParsingTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(cmdFlagParserFriendlyInputMatchingTest);
        if (runTest(tInfo, cmdFlagParserFriendlyInputMatchingTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(cmdFlagParserMatchingEdgecasesTest);
        if (runTest(tInfo, cmdFlagParserMatchingEdgecasesTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(cmdParserValidationRulesTest);
        if (runTest(tInfo, cmdParserValidationRulesTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(cmdParserAliasTest);
        if (runTest(tInfo, cmdParserAliasTest) != 0) { retCode = -1; }
    };

    i32 ret = 0;
    runForAllGlobalAllocatorVariants(runTests, ret);

    {
        constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE * 15;
        char buf[BUFFER_SIZE];
        USE_STACK_BASED_BUMP_ALLOCATOR_FOR_BLOCK_SCOPE(buf, BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "STACK BASED BUMP Allocator", ret);
    }

    {
        constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE * 8;
        USE_CUSTOM_ARENA_ALLOCATOR_FOR_FOR_BLOCK_SCOPE(BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "CUSTOM ARENA Allocator", ret);
    }

    return ret;
}
