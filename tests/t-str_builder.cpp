#include "t-index.h"

template<typename TAllocator>
constexpr i32 initializeStrBuilderTest() {
    using StrBuilder = core::StrBuilder<TAllocator>;

    {
        StrBuilder s1;
        CT_CHECK(s1.len() == 0);
        CT_CHECK(s1.cap() == 0);
        CT_CHECK(s1.byteLen() == 0);
        CT_CHECK(s1.byteCap() == 0);
        CT_CHECK(s1.empty());
        CT_CHECK(s1.view().buff == nullptr);
    }

    {
        StrBuilder str(2);
        CT_CHECK(str.len() == 2);
        CT_CHECK(str.byteLen() == 2 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str.cap() == 3);
        CT_CHECK(str.byteCap() == 3 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(!str.empty());

        CT_CHECK(str.first() == core::term_char);
        CT_CHECK(str.last() == core::term_char);
        CT_CHECK(str[0] == core::term_char);
        CT_CHECK(str[1] == core::term_char);
        CT_CHECK(str.at(0) == core::term_char);
        CT_CHECK(str.at(1) == core::term_char);
    }

    {
        StrBuilder str(2, 5);
        CT_CHECK(str.len() == 2);
        CT_CHECK(str.byteLen() == 2 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str.cap() == 5);
        CT_CHECK(str.byteCap() == 5 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(!str.empty());

        CT_CHECK(str.first() == core::term_char);
        CT_CHECK(str[0] == core::term_char);
        CT_CHECK(str[1] == core::term_char);
        CT_CHECK(str.at(0) == core::term_char);
        CT_CHECK(str.at(1) == core::term_char);
    }

    {
        StrBuilder str("hello");

        CT_CHECK(str.len() == 5);
        CT_CHECK(str.byteLen() == 5 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str.cap() == 6);
        CT_CHECK(str.byteCap() == 6 * sizeof(typename StrBuilder::DataType));

        CT_CHECK(str.first() == 'h');
        CT_CHECK(str.last() == 'o');
        CT_CHECK(str[0] == 'h');
        CT_CHECK(str[1] == 'e');
        CT_CHECK(str[2] == 'l');
        CT_CHECK(str[3] == 'l');
        CT_CHECK(str[4] == 'o');
        CT_CHECK(str.at(0) == 'h');
        CT_CHECK(str.at(1) == 'e');
        CT_CHECK(str.at(2) == 'l');
        CT_CHECK(str.at(3) == 'l');
        CT_CHECK(str.at(4) == 'o');

        auto sview = str.view();
        CT_CHECK(core::cptrEq(sview.buff, "hello", sview.len()));
    }

    {
        StrBuilder str;
        auto prevCap = str.cap();

        str = "hello";

        CT_CHECK(str.len() == 5);
        CT_CHECK(str.byteLen() == 5 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str.cap() == prevCap*2 + core::cptrLen("hello") + 1);
        CT_CHECK(str.byteCap() == (prevCap*2 + core::cptrLen("hello") + 1) * sizeof(typename StrBuilder::DataType));

        CT_CHECK(str.first() == 'h');
        CT_CHECK(str.last() == 'o');
        CT_CHECK(str[0] == 'h');
        CT_CHECK(str[1] == 'e');
        CT_CHECK(str[2] == 'l');
        CT_CHECK(str[3] == 'l');
        CT_CHECK(str[4] == 'o');
        CT_CHECK(str.at(0) == 'h');
        CT_CHECK(str.at(1) == 'e');
        CT_CHECK(str.at(2) == 'l');
        CT_CHECK(str.at(3) == 'l');
        CT_CHECK(str.at(4) == 'o');

        auto sview = str.view();
        CT_CHECK(core::cptrEq(sview.buff, "hello", sview.len()));
        CT_CHECK(core::cptrLen(sview.buff) == sview.len(), "string view should be correctly null terminated");

        constexpr const char* testMsg = "should not memory leak this";
        prevCap = str.cap();
        str = testMsg;
        CT_CHECK(str.len() == 27);
        CT_CHECK(str.byteLen() == 27 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str.cap() == prevCap*2 + core::cptrLen(testMsg) + 1);
        CT_CHECK(str.byteCap() == (prevCap*2 + core::cptrLen(testMsg) + 1) * sizeof(typename StrBuilder::DataType));

        sview = str.view();
        CT_CHECK(core::cptrEq(sview.buff, testMsg, sview.len()));
        CT_CHECK(core::cptrLen(sview.buff) == sview.len(), "string view should be correctly null terminated");
    }

    return 0;
}

template<typename TAllocator>
constexpr i32 moveAndCopyStrBuilderTest() {
    using StrBuilder = core::StrBuilder<TAllocator>;

    {
        StrBuilder str1("hello");
        StrBuilder str2 = std::move(str1);
        StrBuilder str3;
        StrBuilder str4;

        CT_CHECK(str1.len() == 0);
        CT_CHECK(str1.byteLen() == 0);
        CT_CHECK(str1.cap() == 0);
        CT_CHECK(str1.byteCap() == 0);
        CT_CHECK(str1.empty());
        CT_CHECK(str1.view().buff == nullptr);

        CT_CHECK(str2.len() == 5);
        CT_CHECK(str2.byteLen() == 5 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str2.cap() == 6);
        CT_CHECK(str2.byteCap() == 6 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(!str2.empty());
        CT_CHECK(core::cptrEq(str2.view().buff, "hello", str2.view().len()));
        CT_CHECK(core::cptrLen(str2.view().buff) == str2.view().len(), "string view should be correctly null terminated");

        str3 = std::move(str2);
        str4 = str3.copy();

        CT_CHECK(str2.len() == 0);
        CT_CHECK(str2.byteLen() == 0);
        CT_CHECK(str2.cap() == 0);
        CT_CHECK(str2.byteCap() == 0);
        CT_CHECK(str2.empty());
        CT_CHECK(str2.view().buff == nullptr);

        CT_CHECK(str3.len() == 5);
        CT_CHECK(str3.byteLen() == 5 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str3.cap() == 6);
        CT_CHECK(str3.byteCap() == 6 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(!str3.empty());
        CT_CHECK(core::cptrEq(str3.view().buff, "hello", str3.view().len()));
        CT_CHECK(core::cptrLen(str3.view().buff) == str3.view().len(), "string view should be correctly null terminated");

        CT_CHECK(str4.len() == 5);
        CT_CHECK(str4.byteLen() == 5 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(str4.cap() == 6);
        CT_CHECK(str4.byteCap() == 6 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(!str4.empty());
        CT_CHECK(core::cptrEq(str4.view().buff, "hello", str4.view().len()));
        CT_CHECK(core::cptrLen(str4.view().buff) == str4.view().len(), "string view should be correctly null terminated");

        CT_CHECK(str3.view().buff != str4.view().buff);
    }

    {
        // Move assignment of self.
        StrBuilder a = "hello";
        a = std::move(a);

        CT_CHECK(a.len() == 5);
        CT_CHECK(a.byteLen() == 5 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(a.cap() == 6);
        CT_CHECK(a.byteCap() == 6 * sizeof(typename StrBuilder::DataType));
        CT_CHECK(!a.empty());
        CT_CHECK(core::cptrEq(a.view().buff, "hello", a.view().len()));
    }

    {
        // Assigning char pointer when there is enough memory to fit it and when ther isn't.

        constexpr const char* testMsg = "Hello world!";
        StrBuilder a;
        auto prevCap = a.cap();

        a = testMsg;

        CT_CHECK(a.len() == core::cptrLen(testMsg));
        CT_CHECK(a.byteLen() == core::cptrLen(testMsg) * sizeof(typename StrBuilder::DataType));
        CT_CHECK(a.cap() == prevCap*2 + core::cptrLen(testMsg) + 1);
        CT_CHECK(a.byteCap() == (prevCap*2 + core::cptrLen(testMsg) + 1) * sizeof(typename StrBuilder::DataType));

        constexpr const char* testMsg2 = "This should fit!";
        prevCap = a.cap();
        a = testMsg2;
        CT_CHECK(a.len() == core::cptrLen(testMsg2));
        CT_CHECK(a.cap() == prevCap*2 + core::cptrLen(testMsg2) + 1, "should not resize when char ptr fits in current capacity");
        CT_CHECK(a.byteCap() == (prevCap*2 + core::cptrLen(testMsg2) + 1) * sizeof(typename StrBuilder::DataType));
    }

    return 0;
}

template<typename TAllocator>
i32 ensureCapStrBuilderTest() {
    using StrBuilder = core::StrBuilder<TAllocator>;

    StrBuilder s;
    CT_CHECK(s.len() == 0);
    CT_CHECK(s.cap() == 0);
    CT_CHECK(s.view().buff == nullptr);
    CT_CHECK(s.empty());

    s.ensureCap(10);
    CT_CHECK(s.len() == 0);
    CT_CHECK(s.cap() == 10);
    CT_CHECK(s.view().buff != nullptr);
    CT_CHECK(s.empty());

    s.ensureCap(2);
    CT_CHECK(s.len() == 0);
    CT_CHECK(s.cap() == 10);
    CT_CHECK(s.view().buff != nullptr);
    CT_CHECK(s.empty());

    return 0;
}

template<typename TAllocator>
i32 fillStrBuilderTest() {
    using StrBuilder = core::StrBuilder<TAllocator>;

    {
        StrBuilder s;
        s.fill('a'); // should not crash
        CT_CHECK(s.len() == 0);
        CT_CHECK(s.cap() == 0);
        CT_CHECK(s.view().buff == nullptr);
    }

    {
        StrBuilder s(10);

        s.fill(0);
        for (addr_size i = 0; i < s.len(); ++i) {
            CT_CHECK(s[i] == 0);
        }

        s.fill('a');
        for (addr_size i = 0; i < s.len(); ++i) {
            CT_CHECK(s[i] == 'a');
        }
    }

    {
        StrBuilder s(1, 5);

        s.fill(0);
        for (addr_size i = 0; i < s.len(); ++i) {
            CT_CHECK(s[i] == 0);
            CT_CHECK(i < 1, "Fill should use the length of the string builder, not the capacity.");
        }

        s.fill(1);
        for (addr_size i = 0; i < s.len(); ++i) {
            CT_CHECK(s[i] == 1);
            CT_CHECK(i < 1, "Fill should use the length of the string builder, not the capacity.");
        }
    }

    return 0;
}

template<typename TAllocator>
i32 appendStrBuilderTest() {
    using StrBuilder = core::StrBuilder<TAllocator>;

    {
        StrBuilder s;
        s.append('a'); // should not crash
        CT_CHECK(s.len() == 1);
        CT_CHECK(s.cap() == 2);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s[0] == 'a');
    }

    {
        StrBuilder s(2);

        s.append('b');
        CT_CHECK(s.len() == 3);
        CT_CHECK(s.cap() == 6);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s[0] == 0);
        CT_CHECK(s[1] == 0);
        CT_CHECK(s[2] == 'b');

        s.append('a');
        CT_CHECK(s.len() == 4);
        CT_CHECK(s.cap() == 6);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s[0] == 0);
        CT_CHECK(s[1] == 0);
        CT_CHECK(s[2] == 'b');
        CT_CHECK(s[3] == 'a');
    }

    {
        StrBuilder s(1, 5);

        s.append('b');
        CT_CHECK(s.len() == 2);
        CT_CHECK(s.cap() == 5);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s[0] == 0);
        CT_CHECK(s[1] == 'b');

        s.append('a');
        CT_CHECK(s.len() == 3);
        CT_CHECK(s.cap() == 5);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s[0] == 0);
        CT_CHECK(s[1] == 'b');
        CT_CHECK(s[2] == 'a');
    }

    {
        // Appending past the specified capacity.
        StrBuilder s(0, 2);

        s.append('a').append('b').append('c');

        CT_CHECK(s.len() == 3);
        CT_CHECK(s.cap() == 4);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s[0] == 'a');
        CT_CHECK(s[1] == 'b');
        CT_CHECK(s[2] == 'c');

        s.clear();

        CT_CHECK(s.len() == 0);
        CT_CHECK(s.cap() == 4);
        CT_CHECK(s.view().buff != nullptr);

        s.append('e').append('f').append('g');

        CT_CHECK(s.len() == 3);
        CT_CHECK(s.cap() == 4);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s[0] == 'e');
        CT_CHECK(s[1] == 'f');
        CT_CHECK(s[2] == 'g');
    }

    {
        // Append whole string.
        StrBuilder s(0, 2);

        {
            auto prevCap = s.cap();
            s.append("abc");
            CT_CHECK(s.len() == 3);
            CT_CHECK(s.cap() == prevCap * 2 + core::cptrLen("abc") + 1);
            CT_CHECK(s.view().buff != nullptr);
            CT_CHECK(s[0] == 'a');
            CT_CHECK(s[1] == 'b');
            CT_CHECK(s[2] == 'c');

            s.clear();
        }
        {
            auto prevCap = s.cap();
            char buf[] = "efg";
            s.append(buf);
            CT_CHECK(s.len() == 3);
            CT_CHECK(s.cap() == prevCap, "No need to resize, there is enough space.");
            CT_CHECK(s.view().buff != buf);
            CT_CHECK(s.view().len() == 3);
            CT_CHECK(core::cptrEq(s.view().buff, buf, s.view().len()));

            s.clear();

            CT_CHECK(core::cptrEq(buf, "efg", 3), "clear must not affect the original buffer");
        }
        {
            auto prevCap = s.cap();
            const char buf[] = "higklmn";
            s.append(buf, 5);

            CT_CHECK(s.len() == 5);
            CT_CHECK(s.cap() == prevCap, "No need to resize, there is enough space.");
            CT_CHECK(s.view().buff != nullptr);
            CT_CHECK(s.view().len() == 5);
            CT_CHECK(core::cptrEq(s.view().buff, buf, s.view().len()));
            CT_CHECK(core::cptrLen(s.view().buff) == core::cptrLen("higkl"), "string view should be null terminated");

            s.append("opq");

            CT_CHECK(s.len() == 8);
            CT_CHECK(s.cap() == prevCap * 2, "Should just double the capacity.");
            CT_CHECK(s.view().buff != nullptr);
            CT_CHECK(s.view().len() == 8);
            CT_CHECK(s[0] == 'h');
            CT_CHECK(s[1] == 'i');
            CT_CHECK(s[2] == 'g');
            CT_CHECK(s[3] == 'k');
            CT_CHECK(s[4] == 'l');
            CT_CHECK(s[5] == 'o');
            CT_CHECK(s[6] == 'p');
            CT_CHECK(s[7] == 'q');
            CT_CHECK(core::cptrLen(s.view().buff) == core::cptrLen("higklopq"), "string view should be null terminated");

            CT_CHECK(core::cptrEq(buf, "higklmn", core::cptrLen(buf)), "append must not affect the original buffer");
            s.clear();
            CT_CHECK(core::cptrEq(buf, "higklmn", core::cptrLen(buf)), "clear must not affect the original buffer");
        }
    }

    return 0;
}

template<typename TAllocator>
i32 takeAndStealStrBuilderTest() {
    using StrBuilder = core::StrBuilder<TAllocator>;

    {
        // Basic take ownership.

        constexpr i32 allocatedSize = 25;
        char* data = reinterpret_cast<char*>(TAllocator::alloc(allocatedSize * sizeof(char)));
        core::memset(data, 0, allocatedSize);
        core::memset(data, 'a', allocatedSize - 1);

        StrBuilder s;
        s.reset(&data);
        CT_CHECK(data == nullptr);
        CT_CHECK(s.len() == allocatedSize - 1);
        CT_CHECK(s.cap() == allocatedSize);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s.view().len() == allocatedSize - 1);

        for (addr_size i = 0; i < allocatedSize - 1; ++i) { CT_CHECK(s[i] == 'a'); }
    }

    {
        // Take ownership twice.

        constexpr i32 allocatedSize = 17;
        char* data = reinterpret_cast<char*>(TAllocator::alloc(allocatedSize * sizeof(char)));
        core::memset(data, 0, allocatedSize);
        core::memset(data, 'a', allocatedSize - 1);

        StrBuilder s(0, 5);
        s.reset(&data);
        CT_CHECK(data == nullptr);
        CT_CHECK(s.len() == allocatedSize - 1);
        CT_CHECK(s.cap() == allocatedSize );
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s.view().len() == allocatedSize - 1);

        for (addr_size i = 0; i < allocatedSize - 1; ++i) { CT_CHECK(s[i] == 'a'); }

        char* data2 = reinterpret_cast<char*>(TAllocator::alloc(allocatedSize * 2 * sizeof(char)));
        core::memset(data2, 0, allocatedSize * 2);
        core::memset(data2, 'b', allocatedSize * 2 - 1);

        s.reset(&data2);
        CT_CHECK(data2 == nullptr);
        CT_CHECK(s.len() == allocatedSize * 2 - 1);
        CT_CHECK(s.cap() == allocatedSize * 2);
        CT_CHECK(s.view().buff != nullptr);
        CT_CHECK(s.view().len() == allocatedSize * 2 - 1);

        for (addr_size i = 0; i < allocatedSize * 2 - 1; ++i) { CT_CHECK(s[i] == 'b'); }
    }

    {
        // Basic steal ownership.

        StrBuilder s(0, 5);
        s.append("abcde");

        addr_size dataLen = 0;
        char* data = s.steal(dataLen);

        CT_CHECK(data != nullptr);
        CT_CHECK(core::cptrEq(data, "abcde", 5));

        CT_CHECK(s.len() == 0);
        CT_CHECK(s.cap() == 0);
        CT_CHECK(s.view().buff == nullptr);
        CT_CHECK(s.view().len() == 0);

        // The user code is now responsible for freeing the data.
        TAllocator::free(data, dataLen * sizeof(char));
    }

    return 0;
}

template<typename TAllocator>
i32 specialCasesRelatedToNullTerminationStrBuilderTest() {
    using StrBuilder = core::StrBuilder<TAllocator>;

    {
        StrBuilder s(0, 3);
        CT_CHECK(core::cptrLen(s.view().buff) == 0);
        s.append('a');
        CT_CHECK(core::cptrLen(s.view().buff) == 1);
        s.append('b');
        CT_CHECK(core::cptrLen(s.view().buff) == 2);
        s.append('c');
        CT_CHECK(core::cptrLen(s.view().buff) == 3);
        s.append('d');
        CT_CHECK(core::cptrLen(s.view().buff) == 4);
        s.append('e');
        CT_CHECK(core::cptrLen(s.view().buff) == 5);
        s.append('f');
        CT_CHECK(core::cptrLen(s.view().buff) == 6);
        s.append('g');
        CT_CHECK(core::cptrLen(s.view().buff) == 7);
    }

    {
        // Same length and cap. Cap should till be +1.
        StrBuilder s(1, 1);
        s[0] = 'b'; // set the first char.

        CT_CHECK(s.len() == 1);
        CT_CHECK(s.cap() == 2);
        s.append('a');
        CT_CHECK(s.len() == 2);
        CT_CHECK(s.cap() == 4);
        CT_CHECK(core::cptrLen(s.view().buff) == 2);
        CT_CHECK(s[0] == 'b');
        CT_CHECK(s[1] == 'a');
    }

    {
        // Appending strings that have a larger length than the capacity.
        constexpr const char* msg1 = "large-ish volume of text";
        StrBuilder s;
        s.append(msg1, core::cptrLen(msg1));
        CT_CHECK(s.len() == core::cptrLen(msg1));
        CT_CHECK(s.cap() == core::cptrLen(msg1) + 1, "Should resize with the length of the large-ish string.");
        addr_size currCap = s.cap();

        constexpr const char* msg2 = "small msg";
        s.append(msg2);
        CT_CHECK(s.len() == core::cptrLen(msg1) + core::cptrLen(msg2));
        CT_CHECK(s.cap() == currCap * 2);
    }

    {
        // Appending after clearing. Should be cheap and should maintain null termination.
        StrBuilder s;
        s.append("abc");
        CT_CHECK(s.len() == 3);
        CT_CHECK(s.cap() == 4);

        s.clear();
        CT_CHECK(s.len() == 0);
        CT_CHECK(s.cap() == 4);

        s.append('d');
        CT_CHECK(s.len() == 1);
        CT_CHECK(s.cap() == 4);
        CT_CHECK(core::cptrLen(s.view().buff) == 1);

        s.clear();
        CT_CHECK(s.len() == 0);
        CT_CHECK(s.cap() == 4);

        s.append("ab");
        CT_CHECK(s.len() == 2);
        CT_CHECK(s.cap() == 4);
        CT_CHECK(core::cptrLen(s.view().buff) == 2);
    }

    return 0;
}

template <typename TAllocator>
i32 runStrBuilderInArrayAppendRValueBUGTest() {
    using Sb = core::StrBuilder<TAllocator>;
    using SbArr = core::Arr<Sb, TAllocator>;

    SbArr arr;

    arr.append(Sb("hello"));

    Sb& rFromArr = arr[0];
    Sb newValue = rFromArr.copy();
    newValue.append(" world!");
    newValue[0] = 'H';

    arr.append(std::move(newValue));

    // NOTE: Remember that after this append the array will be reallocated and the rFromArr reference will become invalid.

    CT_CHECK(arr.len() == 2);
    CT_CHECK(arr[0].len() == 5);
    CT_CHECK(arr[1].len() == 12);

    CT_CHECK(core::cptrEq(arr[0].view().buff, "hello", 5));
    CT_CHECK(core::cptrEq(arr[1].view().buff, "Hello world!", 12));

    return 0;
}

i32 runStrBuilderTestsSuite() {
    constexpr addr_size BUFF_SIZE = core::KILOBYTE;
    char buf[BUFF_SIZE];

    core::StdAllocator::init(nullptr);
    core::StdStatsAllocator::init(nullptr);
    core::BumpAllocator::init(nullptr, buf, BUFF_SIZE);

    auto checkLeaks = []() {
        CT_CHECK(core::StdAllocator::usedMem() == 0);
        CT_CHECK(core::StdStatsAllocator::usedMem() == 0, "Memory leak detected!");
        CT_CHECK(core::BumpAllocator::usedMem() == 0);
    };

    {
        RunTest_DisplayMemAllocs(initializeStrBuilderTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(initializeStrBuilderTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(initializeStrBuilderTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(moveAndCopyStrBuilderTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(moveAndCopyStrBuilderTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(moveAndCopyStrBuilderTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(ensureCapStrBuilderTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(ensureCapStrBuilderTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(ensureCapStrBuilderTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(fillStrBuilderTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(fillStrBuilderTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(fillStrBuilderTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(appendStrBuilderTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(appendStrBuilderTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(appendStrBuilderTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(takeAndStealStrBuilderTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(takeAndStealStrBuilderTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(takeAndStealStrBuilderTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(specialCasesRelatedToNullTerminationStrBuilderTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(specialCasesRelatedToNullTerminationStrBuilderTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(specialCasesRelatedToNullTerminationStrBuilderTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(runStrBuilderInArrayAppendRValueBUGTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(runStrBuilderInArrayAppendRValueBUGTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(runStrBuilderInArrayAppendRValueBUGTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }

    return 0;
}
