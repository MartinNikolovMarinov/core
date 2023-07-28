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

        core::str_view_tmp sview = str.view();
        Assert(core::cptr_eq(sview.buff, "hello", sview.len));

        char buff[sizeof("hello") + 1] = {};
        str.copy_to(buff);
        Assert(core::cptr_cmp(buff, "hello") == 0);
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

        core::str_view_tmp sview = str.view();
        Assert(core::cptr_eq(sview.buff, "hello", sview.len));

        char buff[sizeof("hello") + 1] = {};
        str.copy_to(buff);
        Assert(core::cptr_cmp(buff, "hello") == 0);
    }

    return 0;
}
