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
        Assert(str.cap() == 3);
        Assert(str.byte_cap() == 3 * sizeof(typename str_builder::data_type));
        Assert(!str.empty());

        Assert(str.first() == core::term_char);
        Assert(str.last() == core::term_char);
        Assert(str[0] == core::term_char);
        Assert(str[1] == core::term_char);
        Assert(str.at(0) == core::term_char);
        Assert(str.at(1) == core::term_char);
    }

    {
        str_builder str(2, 5);
        Assert(str.len() == 2);
        Assert(str.byte_len() == 2 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == 5);
        Assert(str.byte_cap() == 5 * sizeof(typename str_builder::data_type));
        Assert(!str.empty());

        Assert(str.first() == core::term_char);
        Assert(str[0] == core::term_char);
        Assert(str[1] == core::term_char);
        Assert(str.at(0) == core::term_char);
        Assert(str.at(1) == core::term_char);
    }

    {
        str_builder str("hello");

        Assert(str.len() == 5);
        Assert(str.byte_len() == 5 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == 6);
        Assert(str.byte_cap() == 6 * sizeof(typename str_builder::data_type));

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
        auto prevCap = str.cap();

        str = "hello";

        Assert(str.len() == 5);
        Assert(str.byte_len() == 5 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == prevCap*2 + core::cptr_len("hello") + 1);
        Assert(str.byte_cap() == (prevCap*2 + core::cptr_len("hello") + 1) * sizeof(typename str_builder::data_type));

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
        Assert(core::cptr_len(sview.buff) == sview.len, "string view should be correctly null terminated");

        constexpr const char* testMsg = "should not memory leak this";
        prevCap = str.cap();
        str = testMsg;
        Assert(str.len() == 27);
        Assert(str.byte_len() == 27 * sizeof(typename str_builder::data_type));
        Assert(str.cap() == prevCap*2 + core::cptr_len(testMsg) + 1);
        Assert(str.byte_cap() == (prevCap*2 + core::cptr_len(testMsg) + 1) * sizeof(typename str_builder::data_type));

        sview = str.view();
        Assert(core::cptr_eq(sview.buff, testMsg, sview.len));
        Assert(core::cptr_len(sview.buff) == sview.len, "string view should be correctly null terminated");
    }

    return 0;
}

template<typename TAllocator>
constexpr i32 move_and_copy_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    {
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
        Assert(str2.cap() == 6);
        Assert(str2.byte_cap() == 6 * sizeof(typename str_builder::data_type));
        Assert(!str2.empty());
        Assert(core::cptr_eq(str2.view().buff, "hello", str2.view().len));
        Assert(core::cptr_len(str2.view().buff) == str2.view().len, "string view should be correctly null terminated");

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
        Assert(str3.cap() == 6);
        Assert(str3.byte_cap() == 6 * sizeof(typename str_builder::data_type));
        Assert(!str3.empty());
        Assert(core::cptr_eq(str3.view().buff, "hello", str3.view().len));
        Assert(core::cptr_len(str3.view().buff) == str3.view().len, "string view should be correctly null terminated");

        Assert(str4.len() == 5);
        Assert(str4.byte_len() == 5 * sizeof(typename str_builder::data_type));
        Assert(str4.cap() == 6);
        Assert(str4.byte_cap() == 6 * sizeof(typename str_builder::data_type));
        Assert(!str4.empty());
        Assert(core::cptr_eq(str4.view().buff, "hello", str4.view().len));
        Assert(core::cptr_len(str4.view().buff) == str4.view().len, "string view should be correctly null terminated");

        Assert(str3.view().buff != str4.view().buff);
    }

    {
        // Move assignment of self.
        str_builder a = "hello";
        a = core::move(a);

        Assert(a.len() == 5);
        Assert(a.byte_len() == 5 * sizeof(typename str_builder::data_type));
        Assert(a.cap() == 6);
        Assert(a.byte_cap() == 6 * sizeof(typename str_builder::data_type));
        Assert(!a.empty());
        Assert(core::cptr_eq(a.view().buff, "hello", a.view().len));
    }

    {
        // Assigning char pointer when there is enough memory to fit it and when ther isn't.

        constexpr const char* testMsg = "Hello world!";
        str_builder a;
        auto prevCap = a.cap();

        a = testMsg;

        Assert(a.len() == core::cptr_len(testMsg));
        Assert(a.byte_len() == core::cptr_len(testMsg) * sizeof(typename str_builder::data_type));
        Assert(a.cap() == prevCap*2 + core::cptr_len(testMsg) + 1);
        Assert(a.byte_cap() == (prevCap*2 + core::cptr_len(testMsg) + 1) * sizeof(typename str_builder::data_type));

        constexpr const char* testMsg2 = "This should fit!";
        prevCap = a.cap();
        a = testMsg2;
        Assert(a.len() == core::cptr_len(testMsg2));
        Assert(a.cap() == prevCap*2 + core::cptr_len(testMsg2) + 1, "should not resize when char ptr fits in current capacity");
        Assert(a.byte_cap() == (prevCap*2 + core::cptr_len(testMsg2) + 1) * sizeof(typename str_builder::data_type));
    }

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
        Assert(s.cap() == 2);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 'a');
    }

    {
        str_builder s(2);

        s.append('b');
        Assert(s.len() == 3);
        Assert(s.cap() == 6);
        Assert(s.view().buff != nullptr);
        Assert(s[0] == 0);
        Assert(s[1] == 0);
        Assert(s[2] == 'b');

        s.append('a');
        Assert(s.len() == 4);
        Assert(s.cap() == 6);
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

    {
        // Append whole string.
        str_builder s(0, 2);

        {
            auto prevCap = s.cap();
            s.append("abc");
            Assert(s.len() == 3);
            Assert(s.cap() == prevCap * 2 + core::cptr_len("abc") + 1);
            Assert(s.view().buff != nullptr);
            Assert(s[0] == 'a');
            Assert(s[1] == 'b');
            Assert(s[2] == 'c');

            s.clear();
        }
        {
            auto prevCap = s.cap();
            char buf[] = "efg";
            s.append(buf);
            Assert(s.len() == 3);
            Assert(s.cap() == prevCap, "No need to resize, there is enough space.");
            Assert(s.view().buff != buf);
            Assert(s.view().len == 3);
            Assert(core::cptr_eq(s.view().buff, buf, s.view().len));

            s.clear();

            Assert(core::cptr_eq(buf, "efg", 3), "clear must not affect the original buffer");
        }
        {
            auto prevCap = s.cap();
            const char buf[] = "higklmn";
            s.append(buf, 5);

            Assert(s.len() == 5);
            Assert(s.cap() == prevCap, "No need to resize, there is enough space.");
            Assert(s.view().buff != nullptr);
            Assert(s.view().len == 5);
            Assert(core::cptr_eq(s.view().buff, buf, s.view().len));
            Assert(core::cptr_len(s.view().buff) == core::cptr_len("higkl"), "string view should be null terminated");

            s.append("opq");

            Assert(s.len() == 8);
            Assert(s.cap() == prevCap * 2, "Should just double the capacity.");
            Assert(s.view().buff != nullptr);
            Assert(s.view().len == 8);
            Assert(s[0] == 'h');
            Assert(s[1] == 'i');
            Assert(s[2] == 'g');
            Assert(s[3] == 'k');
            Assert(s[4] == 'l');
            Assert(s[5] == 'o');
            Assert(s[6] == 'p');
            Assert(s[7] == 'q');
            Assert(core::cptr_len(s.view().buff) == core::cptr_len("higklopq"), "string view should be null terminated");

            Assert(core::cptr_eq(buf, "higklmn", core::cptr_len(buf)), "append must not affect the original buffer");
            s.clear();
            Assert(core::cptr_eq(buf, "higklmn", core::cptr_len(buf)), "clear must not affect the original buffer");
        }
    }

    return 0;
}

template<typename TAllocator>
i32 take_and_steal_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    {
        // Basic take ownership.

        constexpr i32 allocatedSize = 25;
        char* data = reinterpret_cast<char*>(core::alloc<TAllocator>(allocatedSize * sizeof(char)));
        core::memset(data, 0, allocatedSize);
        core::memset(data, 'a', allocatedSize - 1);

        str_builder s;
        s.take_ownership_from(&data);
        Assert(data == nullptr);
        Assert(s.len() == allocatedSize - 1);
        Assert(s.cap() == allocatedSize);
        Assert(s.view().buff != nullptr);
        Assert(s.view().len == allocatedSize - 1);

        for (addr_size i = 0; i < allocatedSize - 1; ++i) { Assert(s[i] == 'a'); }
    }

    {
        // Take ownership twice.

        constexpr i32 allocatedSize = 17;
        char* data = reinterpret_cast<char*>(core::alloc<TAllocator>(allocatedSize * sizeof(char)));
        core::memset(data, 0, allocatedSize);
        core::memset(data, 'a', allocatedSize - 1);

        str_builder s(0, 5);
        s.take_ownership_from(&data);
        Assert(data == nullptr);
        Assert(s.len() == allocatedSize - 1);
        Assert(s.cap() == allocatedSize );
        Assert(s.view().buff != nullptr);
        Assert(s.view().len == allocatedSize - 1);

        for (addr_size i = 0; i < allocatedSize - 1; ++i) { Assert(s[i] == 'a'); }

        char* data2 = reinterpret_cast<char*>(core::alloc<TAllocator>(allocatedSize * 2 * sizeof(char)));
        core::memset(data2, 0, allocatedSize * 2);
        core::memset(data2, 'b', allocatedSize * 2 - 1);

        s.take_ownership_from(&data2);
        Assert(data2 == nullptr);
        Assert(s.len() == allocatedSize * 2 - 1);
        Assert(s.cap() == allocatedSize * 2);
        Assert(s.view().buff != nullptr);
        Assert(s.view().len == allocatedSize * 2 - 1);

        for (addr_size i = 0; i < allocatedSize * 2 - 1; ++i) { Assert(s[i] == 'b'); }
    }

    {
        // Basic steal ownership.

        str_builder s(0, 5);
        s.append("abcde");

        char* data = s.steal_ownership();

        Assert(data != nullptr);
        Assert(core::cptr_eq(data, "abcde", 5));

        Assert(s.len() == 0);
        Assert(s.cap() == 0);
        Assert(s.view().buff == nullptr);
        Assert(s.view().len == 0);

        // The user code is now responsible for freeing the data.
        core::free<TAllocator>(data);
    }

    return 0;
}

template<typename TAllocator>
i32 special_cases_related_to_null_termination_str_builder() {
    using str_builder = core::str_builder<TAllocator>;

    {
        str_builder s(0, 3);
        Assert(core::cptr_len(s.view().buff) == 0);
        s.append('a');
        Assert(core::cptr_len(s.view().buff) == 1);
        s.append('b');
        Assert(core::cptr_len(s.view().buff) == 2);
        s.append('c');
        Assert(core::cptr_len(s.view().buff) == 3);
        s.append('d');
        Assert(core::cptr_len(s.view().buff) == 4);
        s.append('e');
        Assert(core::cptr_len(s.view().buff) == 5);
        s.append('f');
        Assert(core::cptr_len(s.view().buff) == 6);
        s.append('g');
        Assert(core::cptr_len(s.view().buff) == 7);
    }

    {
        // Same length and cap. Cap should till be +1.
        str_builder s(1, 1);
        s[0] = 'b'; // set the first char.

        Assert(s.len() == 1);
        Assert(s.cap() == 2);
        s.append('a');
        Assert(s.len() == 2);
        Assert(s.cap() == 4);
        Assert(core::cptr_len(s.view().buff) == 2);
        Assert(s[0] == 'b');
        Assert(s[1] == 'a');
    }

    {
        // Appending strings that have a larger length than the capacity.
        constexpr const char* msg1 = "large-ish volume of text";
        str_builder s;
        s.append(msg1, core::cptr_len(msg1));
        Assert(s.len() == core::cptr_len(msg1));
        Assert(s.cap() == core::cptr_len(msg1) + 1, "Should resize with the length of the large-ish string.");
        addr_size currCap = s.cap();

        constexpr const char* msg2 = "small msg";
        s.append(msg2);
        Assert(s.len() == core::cptr_len(msg1) + core::cptr_len(msg2));
        Assert(s.cap() == currCap * 2);
    }

    {
        // Appending after clearing. Should be cheap and should maintain null termination.
        str_builder s;
        s.append("abc");
        Assert(s.len() == 3);
        Assert(s.cap() == 4);

        s.clear();
        Assert(s.len() == 0);
        Assert(s.cap() == 4);

        s.append('d');
        Assert(s.len() == 1);
        Assert(s.cap() == 4);
        Assert(core::cptr_len(s.view().buff) == 1);

        s.clear();
        Assert(s.len() == 0);
        Assert(s.cap() == 4);

        s.append("ab");
        Assert(s.len() == 2);
        Assert(s.cap() == 4);
        Assert(core::cptr_len(s.view().buff) == 2);
    }

    return 0;
}
