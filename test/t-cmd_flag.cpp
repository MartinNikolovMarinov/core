template<typename TAllocator>
i32 friendly_input_flag_parser_test() {
    using flag_parser = core::flag_parser<TAllocator>;

    flag_parser parser;

    i32 a = 0;
    i64 b = 0;
    u32 c = 0;
    u64 d = 0;
    parser.flag(&a, core::sv(" int32"));
    parser.flag(&b, core::sv("int64 "));
    parser.flag(&c, core::sv("uint32\t"));
    parser.flag(&d, core::sv("\nuint64"));

    char* cptrArg = nullptr;
    parser.flag(&cptrArg, core::sv("string"));

    bool bool_1 = false;
    bool bool_2 = false;
    bool bool_3 = false;
    bool bool_4 = false;
    bool bool_5 = false;
    bool bool_6 = false;
    bool bool_7 = false;
    bool bool_8 = false;
    parser.flag(&bool_1, core::sv("   bool-1"));
    parser.flag(&bool_2, core::sv("bool-2"));
    parser.flag(&bool_3, core::sv("bool-3 "));
    parser.flag(&bool_4, core::sv("bool-4"));
    parser.flag(&bool_5, core::sv("bool-5   "));
    parser.flag(&bool_6, core::sv("  bool-6"));
    parser.flag(&bool_7, core::sv("bool-7"));
    parser.flag(&bool_8, core::sv("bool-8"));

    f32 fa = 0, fb = 0, fc = 0, fd = 0;
    f64 fe = 0, ff = 0, fg = 0, fh = 0;
    parser.flag(&fa, core::sv("float32-1"));
    parser.flag(&fb, core::sv("float32-2"));
    parser.flag(&fc, core::sv("float32-3"));
    parser.flag(&fd, core::sv("float32-4"));
    parser.flag(&fe, core::sv("float64-5"));
    parser.flag(&ff, core::sv("float64-6"));
    parser.flag(&fg, core::sv("float64-7"));
    parser.flag(&fh, core::sv("float64-8"));

    constexpr i32 ARGC = 44;
    const char* input[ARGC] = {
        "-bool-1 ", " t",
        "-bool-2   ", "T",
        "-bool-3  ", " true",
        "  -bool-4", "  TRUE",
        "--bool-5 ", "True  ",
        "    -bool-6", "1",
        "-bool-7", "false",
        "-bool-8", "zxcasdasd",

        " -int32", "0004",
        "--int64  ", "  -13",
        "\t-uint32", "19 ",
        "   -uint64", "\t99asad",

        "-string", "  banicata   fsa  ",

        "-float32-1", "  1.2",
        "-float32-2", "  .5. ",
        "-float32-3", " 1. ",
        "--float32-4", "  -1.2cvxc",
        "-float64-5", " 1.2.",
        "-float64-6", "7...  ",
        "-float64-7", "-1.2",
        "-float64-8", "00012.000005",
    };
    auto ret = parser.parse(ARGC, input);
    Assert(!ret.has_err());

    // Check integer parsing
    Assert(a == 4);
    Assert(b == -13);
    Assert(c == 19);
    Assert(d == 99);

    // Check string parsing
    Assert(core::cptr_eq(cptrArg, "  banicata   fsa  ", core::cptr_len(cptrArg)));

    // Check boolean parsing
    Assert(bool_1);
    Assert(bool_2);
    Assert(bool_3);
    Assert(bool_4);
    Assert(bool_5);
    Assert(bool_6);
    Assert(!bool_7);
    Assert(!bool_8);

    // Check float parsing
    Assert(core::safe_eq(fa, 1.2f, 0.00001f));
    Assert(core::safe_eq(fb, 0.5f, 0.00001f));
    Assert(core::safe_eq(fc, 1.0f, 0.00001f));
    Assert(core::safe_eq(fd, -1.2f, 0.00001f));
    Assert(core::safe_eq(fe, 1.2, 0.00001));
    Assert(core::safe_eq(ff, 7.0, 0.00001));
    Assert(core::safe_eq(fg, -1.2, 0.00001));
    Assert(core::safe_eq(fh, 12.000005, 0.00001));

    return 0;
}

template<typename TAllocator>
i32 adverse_input_flag_parser_test() {
    using flag_parser = core::flag_parser<TAllocator>;

    {
        flag_parser parser;
        auto res = parser.parse(5, nullptr);
        Assert(res.has_err());
        Assert(res.err() == flag_parser::parse_err::NothingToParse);
    }
    {
        flag_parser parser;
        const char* input[1] = {nullptr};
        auto res = parser.parse(0, input);
        Assert(res.has_err());
        Assert(res.err() == flag_parser::parse_err::NothingToParse);
    }
    {
        flag_parser parser;
        const char* input[1] = {nullptr};
        auto res = parser.parse(1, input);
        Assert(!res.has_err());
    }
    {
        // NOTE: This is a segfault and I can't prevent it, unless I make the API less convenient.
        // const char* input[1] = {nullptr};
        // auto res = parser.parse(2, input);
        // Assert(!res.has_err());
    }
    {
        flag_parser parser;
        const char* input[1] = {"-int32 5"};
        auto res = parser.parse(1, input);
        Assert(res.has_err());
        Assert(res.err() == flag_parser::parse_err::UnknownFlag);

        parser.allowUnknownFlags = true;
        auto res2 = parser.parse(1, input);
        Assert(!res2.has_err());

        parser.maxArgLen = 2;
        auto res3 = parser.parse(1, input);
        Assert(res3.has_err());
        Assert(res3.err() == flag_parser::parse_err::ArgLenTooLong);

        i32 a;
        parser.maxArgLen = 500;
        parser.flag(&a, core::sv("not_a_flag"), true);
        auto res4 = parser.parse(1, input);
        Assert(res4.has_err());
        Assert(res4.err() == flag_parser::parse_err::MissingRequiredFlag);

        // overload that same flag to not be required
        parser.flag(&a, core::sv("not_a_flag"), false);
        auto res5 = parser.parse(1, input);
        Assert(!res5.has_err());
    }
    {
        char* a = nullptr;
        flag_parser parser;
        parser.flag(&a, core::sv("str"));
        parser.allowUnknownFlags = true;
        const char* input[10] = {"-asdxczzxcad", "asd", "ascxz", "zxc", "asd", "-dasda", "", "-str", nullptr, "asd" };
        auto res = parser.parse(10, input);
        Assert(!res.has_err());

        Assert(parser.flags.len() == 1);
        Assert(core::cptr_eq(a, "asd", 3));
    }

    return 0;
}

template<typename TAllocator>
i32 custom_rule_flag_parser_test() {
    using flag_parser = core::flag_parser<TAllocator>;

    flag_parser parser;
    i32 a, b;

    {
        parser.flag(&a, core::sv("a"));
        parser.flag(&b, core::sv("b"));
        const char* input[4] = {"-a", "1", "-b", "5" };
        auto res = parser.parse(4, input);
        Assert(!res.has_err());

        // Add some rules

        parser.flag(&a, core::sv("a"), false, [](void* a) -> bool {
            i32 v = *(i32*)a;
            return (v > 0);
        });
        parser.flag(&b, core::sv("b"), false, [](void* b) -> bool {
            i32 v = *(i32*)b;
            return (v <= 10);
        });
    }

    // Check if the rules catch errors, when they should.

    {
        const char* input[4] = {"-a", "1", "-b", "5" };
        auto res = parser.parse(4, input);
        Assert(!res.has_err());
    }
    {
        const char* input[4] = {"-a", "0", "-b", "5" };
        auto res = parser.parse(4, input);
        Assert(res.has_err());
        Assert(res.err() == flag_parser::parse_err::CustomRuleViolation);
    }
    {
        const char* input[4] = {"-a", "1", "-b", "10" };
        auto res = parser.parse(4, input);
        Assert(!res.has_err());
    }
    {
        const char* input[4] = {"-a", "0", "-b", "11" };
        auto res = parser.parse(4, input);
        Assert(res.has_err());
        Assert(res.err() == flag_parser::parse_err::CustomRuleViolation);
    }
    {
        const char* input[4] = {"-a", "1", "-b", "10" };
        auto res = parser.parse(4, input);
        Assert(!res.has_err());
    }
    {
        parser.allowUnknownFlags = true;
        const char* input[2] = {"-a", "1" }; // b is not required, the custom validator should not fail this case!
        auto res = parser.parse(2, input);
        Assert(!res.has_err());
        parser.allowUnknownFlags = false;
    }

    return 0;
}

template <typename TAllocator>
i32 alias_flag_parser_test() {
    using flag_parser = core::flag_parser<TAllocator>;

    {
        flag_parser parser;
        i32 arg1 = -1; 
        bool arg2 = false;
        
        parser.flag(&arg1, core::sv("full_name"));
        parser.flag(&arg2, core::sv("full_name_2"));
        parser.alias(core::sv("full_name"), core::sv("a"));
        parser.alias(core::sv("full_name_2"), core::sv("b"));

        const char* input[4] = { "-a", "1", "-b", "true" };
        auto res = parser.parse(4, input);

        Assert(!res.has_err());
        Assert(arg1 == 1);
        Assert(arg2 == true);
    }

    // Update a flag that has an alias
    {
        flag_parser parser;
        i32 arg1 = -1;
        i64 arg2 = -1;

        parser.flag(&arg1, core::sv("full_name"));
        parser.alias(core::sv("full_name"), core::sv("a"));
        parser.flag(&arg2, core::sv("full_name")); // change the type of the flag

        const char* input[2] = { "-a", "2" };
        auto res = parser.parse(2, input); // setting through the alias should work when the flag is required.

        Assert(!res.has_err());
        Assert(arg1 == -1);
        Assert(arg2 == 2); // the alias should work with the new argument only.
    }

    // Use more than one alias
    {
        flag_parser parser;
        i32 arg1 = -1;

        parser.flag(&arg1, core::sv("full_name")); // change the type of the flag
        parser.alias(core::sv("full_name"), core::sv("a"));
        parser.alias(core::sv("full_name"), core::sv("b"));

        const char* input[2] = { "-a", "2" };
        auto res = parser.parse(2, input);
        Assert(!res.has_err());
        Assert(arg1 == 2);

        arg1 = -1;

        const char* input2[2] = { "-b", "3" };
        auto res2 = parser.parse(2, input2);
        Assert(!res2.has_err());
        Assert(arg1 == 3);

        arg1 = -1;

        const char* input3[4] = { "-a", "4", "-b", "5" };
        auto res3 = parser.parse(4, input3);
        Assert(!res3.has_err());
        Assert(arg1 == 5); // When two aliases for the same flag are used the system should take the last one.
    }

    return 0;
}