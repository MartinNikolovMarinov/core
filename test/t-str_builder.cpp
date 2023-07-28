template<typename TAllocator>
constexpr i32 initalize_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    {
        str_builder s1;
        Assert(s1.len() == 0);
        Assert(s1.cap() == 0);
        Assert(s1.byte_len() == 0);
        Assert(s1.byte_cap() == 0);
        Assert(s1.empty());
        Assert(s1.view().buff == nullptr);
    }

    {
        str_builder str(2);
        Assert(str.len() == 2);
        Assert(str.byte_len() == 2 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == 2);
        Assert(str.byte_cap() == 2 * sizeof(typename str_builder::data_type));
        Assert(!str.empty());

        Assert(str.first() == '\0');
        Assert(str.last() == '\0');
        Assert(str[0] == '\0');
        Assert(str[1] == '\0');
        Assert(str.at(0) == '\0');
        Assert(str.at(1) == '\0');
    }

    {
        str_builder str(2, 5);
        Assert(str.len() == 2);
        Assert(str.byte_len() == 2 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == 5);
        Assert(str.byte_cap() == 5 * sizeof(typename str_builder::data_type));
        Assert(!str.empty());

        Assert(str.first() == '\0');
        Assert(str[0] == '\0');
        Assert(str[1] == '\0');
        Assert(str.at(0) == '\0');
        Assert(str.at(1) == '\0');
    }

    {
        str_builder str("hello");

        Assert(str.len() == 5);
        Assert(str.byte_len() == 5 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == 5);
        Assert(str.byte_cap() == 5 * sizeof(typename str_builder::data_type));

        Assert(str.first() == 'h');
        Assert(str.last() == 'o');
        Assert(str[0] == 'h');
        Assert(str[1] == 'e');
        Assert(str[2] == 'l');
        Assert(str[3] == 'l');
        Assert(str[4] == 'o');
        Assert(str.at(0) == 'h');
        Assert(str.at(1) == 'e');
        Assert(str.at(2) == 'l');
        Assert(str.at(3) == 'l');
        Assert(str.at(4) == 'o');

        auto sview = str.view();
        Assert(core::cptr_eq(sview.buff, "hello", sview.len));
    }

    {
        str_builder str;

        str = "hello";

        Assert(str.len() == 5);
        Assert(str.byte_len() == 5 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == 5);
        Assert(str.byte_cap() == 5 * sizeof(typename str_builder::data_type));

        Assert(str.first() == 'h');
        Assert(str.last() == 'o');
        Assert(str[0] == 'h');
        Assert(str[1] == 'e');
        Assert(str[2] == 'l');
        Assert(str[3] == 'l');
        Assert(str[4] == 'o');
        Assert(str.at(0) == 'h');
        Assert(str.at(1) == 'e');
        Assert(str.at(2) == 'l');
        Assert(str.at(3) == 'l');
        Assert(str.at(4) == 'o');

        auto sview = str.view();
        Assert(core::cptr_eq(sview.buff, "hello", sview.len));

        constexpr const char* testMsg = "should not memory leak this";
        str = testMsg;
        Assert(str.len() == 27);
        Assert(str.byte_len() == 27 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == 27);
        Assert(str.byte_cap() == 27 * sizeof(typename str_builder::data_type));

        sview = str.view();
        Assert(core::cptr_eq(sview.buff, testMsg, sview.len));
    }

    return 0;
}

template<typename TAllocator>
constexpr i32 move_and_copy_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    str_builder str1("hello");
    str_builder str2 = core::move(str1);
    str_builder str3;
    str_builder str4;

    Assert(str1.len() == 0);
    Assert(str1.byte_len() == 0);
    Assert(str1.cap() == 0);
    Assert(str1.byte_cap() == 0);
    Assert(str1.empty());
    Assert(str1.view().buff == nullptr);

    Assert(str2.len() == 5);
    Assert(str2.byte_len() == 5 * sizeof(typename str_builder::data_type));
    Assert(str2.cap() == 5);
    Assert(str2.byte_cap() == 5 * sizeof(typename str_builder::data_type));
    Assert(!str2.empty());
    Assert(core::cptr_eq(str2.view().buff, "hello", str2.view().len));

    str3 = core::move(str2);
    str4 = str3.copy();

    Assert(str2.len() == 0);
    Assert(str2.byte_len() == 0);
    Assert(str2.cap() == 0);
    Assert(str2.byte_cap() == 0);
    Assert(str2.empty());
    Assert(str2.view().buff == nullptr);

    Assert(str3.len() == 5);
    Assert(str3.byte_len() == 5 * sizeof(typename str_builder::data_type));
    Assert(str3.cap() == 5);
    Assert(str3.byte_cap() == 5 * sizeof(typename str_builder::data_type));
    Assert(!str3.empty());
    Assert(core::cptr_eq(str3.view().buff, "hello", str3.view().len));

    Assert(str4.len() == 5);
    Assert(str4.byte_len() == 5 * sizeof(typename str_builder::data_type));
    Assert(str4.cap() == 5);
    Assert(str4.byte_cap() == 5 * sizeof(typename str_builder::data_type));
    Assert(!str4.empty());
    Assert(core::cptr_eq(str4.view().buff, "hello", str4.view().len));

    Assert(str3.view().buff != str4.view().buff);

    return 0;
}

template<typename TAllocator>
i32 resize_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    str_builder s;
    Assert(s.len() == 0);
    Assert(s.cap() == 0);
    Assert(s.view().buff == nullptr);
    Assert(s.empty());

    s.reserve(10);
    Assert(s.len() == 0);
    Assert(s.cap() == 10);
    Assert(s.view().buff != nullptr);
    Assert(s.empty());

    s.reserve(0);
    Assert(s.len() == 0);
    Assert(s.cap() == 0);
    Assert(s.view().buff != nullptr);
    Assert(s.empty());

    return 0;
}

template<typename TAllocator>
i32 fill_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    {
        str_builder s;
        s.fill('a'); // should not crash
        Assert(s.len() == 0);
        Assert(s.cap() == 0);
        Assert(s.view().buff == nullptr);
    }

    {
        str_builder s(10);

        s.fill(0);
        for (i32 i = 0; i < s.len(); ++i) {
            Assert(s[i] == 0);
        }

        s.fill('a');
        for (i32 i = 0; i < s.len(); ++i) {
            Assert(s[i] == 'a');
        }
    }

    {
        str_builder s(1, 5);

        s.fill(0);
        for (i32 i = 0; i < s.len(); ++i) {
            Assert(s[i] == 0);
            Assert(i < 1, "Fill should use the length of the string builder, not the capacity.");
        }

        s.fill(1);
        for (i32 i = 0; i < s.len(); ++i) {
            Assert(s[i] == 1);
            Assert(i < 1, "Fill should use the length of the string builder, not the capacity.");
        }
    }

    return 0;
}

template<typename TAllocator>
i32 append_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    {
        str_builder s;
        s.append('a'); // should not crash
        Assert(s.len() == 1);
        Assert(s.cap() == 1);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 'a');
    }

    {
        str_builder s(2);

        s.append('b');
        Assert(s.len() == 3);
        Assert(s.cap() == 4);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 0);
        Assert(s[1] == 0);
        Assert(s[2] == 'b');

        s.append('a');
        Assert(s.len() == 4);
        Assert(s.cap() == 4);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 0);
        Assert(s[1] == 0);
        Assert(s[2] == 'b');
        Assert(s[3] == 'a');
    }

    {
        str_builder s(1, 5);

        s.append('b');
        Assert(s.len() == 2);
        Assert(s.cap() == 5);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 0);
        Assert(s[1] == 'b');

        s.append('a');
        Assert(s.len() == 3);
        Assert(s.cap() == 5);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 0);
        Assert(s[1] == 'b');
        Assert(s[2] == 'a');
    }

    {
        // Appending past the specified capacity.
        str_builder s(0, 2);

        s.append('a').append('b').append('c');

        Assert(s.len() == 3);
        Assert(s.cap() == 4);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 'a');
        Assert(s[1] == 'b');
        Assert(s[2] == 'c');

        s.clear();

        Assert(s.len() == 0);
        Assert(s.cap() == 4);
        Assert(s.view().buff != nullptr);

        s.append('e').append('f').append('g');

        Assert(s.len() == 3);
        Assert(s.cap() == 4);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 'e');
        Assert(s[1] == 'f');
        Assert(s[2] == 'g');
    }

    return 0;
}


// FIXME: Test take and steal ownership functions.
// FIXME: Test append more.
