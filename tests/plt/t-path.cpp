#include "../t-index.h"

#include "plt/core_path.h"
#include "testing/testing_framework.h"

namespace {

constexpr i32 staticPathBuilderBasicFlowTest() {
    core::StaticPathBuilder<128> pb = {};

    {
        pb.setDirPart("dir"_sv);
        CT_CHECK(pb.fullPathSv().eq("dir/"_sv));
        CT_CHECK(pb.dirPartSv().eq("dir/"_sv));
        CT_CHECK(pb.filePartSv().eq(core::sv("")));
        CT_CHECK(pb.extPartSv().eq(core::sv()));

        pb.appendToDirPath("nested"_sv);
        CT_CHECK(pb.fullPathSv().eq("dir/nested/"_sv));
        CT_CHECK(pb.dirPartSv().eq("dir/nested/"_sv));
        CT_CHECK(pb.filePartSv().eq(core::sv("")));
        CT_CHECK(pb.extPartSv().eq(core::sv()));

        pb.setFilePart("file"_sv);
        CT_CHECK(pb.fullPathSv().eq("dir/nested/file"_sv));
        CT_CHECK(pb.dirPartSv().eq("dir/nested/"_sv));
        CT_CHECK(pb.filePartSv().eq("file"_sv));
        CT_CHECK(pb.extPartSv().eq(core::sv()));

        pb.setExtPart("ext"_sv);
        CT_CHECK(pb.fullPathSv().eq("dir/nested/file.ext"_sv));
        CT_CHECK(pb.dirPartSv().eq("dir/nested/"_sv));
        CT_CHECK(pb.filePartSv().eq("file.ext"_sv));
        CT_CHECK(pb.extPartSv().eq("ext"_sv));

        pb.reset();
        CT_CHECK(pb.fullPathSv().eq(core::sv("")));
        CT_CHECK(pb.dirPartSv().eq(core::sv()));
        CT_CHECK(pb.filePartSv().eq(core::sv("")));
        CT_CHECK(pb.extPartSv().eq(core::sv()));
        CT_CHECK(pb.extPart() == nullptr);
    }

    {
        pb.setDirPart("123"_sv);
        CT_CHECK(pb.fullPathSv().eq("123/"));
        CT_CHECK(pb.dirPartSv().eq("123/"));
        CT_CHECK(pb.filePartSv().empty());
        CT_CHECK(pb.extPartSv().empty());

        pb.appendToDirPath("456"_sv);
        CT_CHECK(pb.fullPathSv().eq("123/456/"));
        CT_CHECK(pb.dirPartSv().eq("123/456/"));
        CT_CHECK(pb.filePartSv().empty());
        CT_CHECK(pb.extPartSv().empty());

        pb.appendToDirPath("78"_sv);
        CT_CHECK(pb.fullPathSv().eq("123/456/78/"));
        CT_CHECK(pb.dirPartSv().eq("123/456/78/"));
        CT_CHECK(pb.filePartSv().empty());
        CT_CHECK(pb.extPartSv().empty());

        pb.setFilePart("910/abcdef"_sv);
        CT_CHECK(pb.fullPathSv().eq("123/456/78/910/abcdef"));
        CT_CHECK(pb.dirPartSv().eq("123/456/78/910/"));
        CT_CHECK(pb.filePartSv().eq("abcdef"));
        CT_CHECK(pb.extPartSv().empty());

        pb.setExtPart("xy"_sv);
        CT_CHECK(pb.fullPathSv().eq("123/456/78/910/abcdef.xy"));
        CT_CHECK(pb.dirPartSv().eq("123/456/78/910/"));
        CT_CHECK(pb.filePartSv().eq("abcdef.xy"));
        CT_CHECK(pb.extPartSv().eq("xy"));

        pb.setExtPart("z"_sv);
        CT_CHECK(pb.fullPathSv().eq("123/456/78/910/abcdef.z"));
        CT_CHECK(pb.dirPartSv().eq("123/456/78/910/"));
        CT_CHECK(pb.filePartSv().eq("abcdef.z"));
        CT_CHECK(pb.extPartSv().eq("z"));

        pb.setDirPart("newDir"_sv);
        CT_CHECK(pb.fullPathSv().eq("newDir/abcdef.z"));
        CT_CHECK(pb.dirPartSv().eq("newDir/"));
        CT_CHECK(pb.filePartSv().eq("abcdef.z"));
        CT_CHECK(pb.extPartSv().eq("z"));
    }

    return 0;
}

constexpr i32 staticPathBuilderSetExtPartFlowTest() {
    core::StaticPathBuilder<128> pb = {};

    pb.setDirPart("directory"_sv);
    pb.setFilePart("file"_sv);
    pb.setExtPart("ext"_sv);
    CT_CHECK(pb.fullPathSv().eq("directory/file.ext"_sv));
    CT_CHECK(pb.dirPartSv().eq("directory/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.ext"_sv));
    CT_CHECK(pb.extPartSv().eq("ext"_sv));

    pb.setExtPart("ext2"_sv);
    CT_CHECK(pb.fullPathSv().eq("directory/file.ext2"_sv));
    CT_CHECK(pb.dirPartSv().eq("directory/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.ext2"_sv));
    CT_CHECK(pb.extPartSv().eq("ext2"_sv));

    pb.setFilePart("over"_sv);
    CT_CHECK(pb.fullPathSv().eq("directory/over"_sv));
    CT_CHECK(pb.dirPartSv().eq("directory/"_sv));
    CT_CHECK(pb.filePartSv().eq("over"_sv));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.setExtPart("ext3"_sv);
    CT_CHECK(pb.fullPathSv().eq("directory/over.ext3"_sv));
    CT_CHECK(pb.dirPartSv().eq("directory/"_sv));
    CT_CHECK(pb.filePartSv().eq("over.ext3"_sv));
    CT_CHECK(pb.extPartSv().eq("ext3"_sv));

    pb.setDirPart("dirover"_sv);
    CT_CHECK(pb.fullPathSv().eq("dirover/over.ext3"_sv));
    CT_CHECK(pb.dirPartSv().eq("dirover/"_sv));
    CT_CHECK(pb.filePartSv().eq("over.ext3"_sv));
    CT_CHECK(pb.extPartSv().eq("ext3"_sv));

    pb.resetFilePart();
    pb.setExtPart("nofileext"_sv);
    CT_CHECK(pb.fullPathSv().eq("dirover/.nofileext"_sv));
    CT_CHECK(pb.dirPartSv().eq("dirover/"_sv));
    CT_CHECK(pb.filePartSv().eq(".nofileext"_sv));
    CT_CHECK(pb.extPartSv().eq("nofileext"_sv));

    pb.reset();
    pb.setExtPart("onlyext"_sv);
    CT_CHECK(pb.fullPathSv().eq(".onlyext"_sv));
    CT_CHECK(pb.dirPartSv().eq(core::sv()));
    CT_CHECK(pb.filePartSv().eq(".onlyext"_sv));
    CT_CHECK(pb.extPartSv().eq("onlyext"_sv));

    pb.reset();
    pb.setExtPart(""_sv);
    CT_CHECK(pb.fullPathSv().eq(core::sv("")));
    CT_CHECK(pb.dirPartSv().eq(core::sv()));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));
    CT_CHECK(pb.extPart() == nullptr);

    pb.reset();
    pb.setFilePart("."_sv);
    pb.setExtPart("."_sv);
    CT_CHECK(pb.fullPathSv().eq("."_sv));
    CT_CHECK(pb.dirPartSv().eq(core::sv()));
    CT_CHECK(pb.filePartSv().eq("."_sv));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    return 0;
}

constexpr i32 staticPathBuilderSetFilePartFlowTest() {
    core::StaticPathBuilder<128> pb = {};

    pb.setDirPart("test"_sv);
    pb.setFilePart("example"_sv);
    CT_CHECK(pb.fullPathSv().eq("test/example"_sv));
    CT_CHECK(pb.dirPartSv().eq("test/"_sv));
    CT_CHECK(pb.filePartSv().eq("example"_sv));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.setFilePart("change"_sv);
    CT_CHECK(pb.fullPathSv().eq("test/change"_sv));
    CT_CHECK(pb.dirPartSv().eq("test/"_sv));
    CT_CHECK(pb.filePartSv().eq("change"_sv));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.reset();
    pb.setFilePart("no_directory_file"_sv);
    CT_CHECK(pb.fullPathSv().eq("no_directory_file"_sv));
    CT_CHECK(pb.dirPartSv().eq(core::sv()));
    CT_CHECK(pb.filePartSv().eq("no_directory_file"_sv));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.setFilePart("k"_sv);
    CT_CHECK(pb.fullPathSv().eq("k"_sv));
    CT_CHECK(pb.dirPartSv().eq(core::sv()));
    CT_CHECK(pb.filePartSv().eq("k"_sv));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.resetFilePart();
    CT_CHECK(pb.fullPathSv().eq(core::sv("")));
    CT_CHECK(pb.dirPartSv().eq(core::sv()));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));
    CT_CHECK(pb.extPart() == nullptr);

    pb.setDirPart("dir"_sv);
    pb.setFilePart("file_name"_sv);
    pb.resetFilePart();
    CT_CHECK(pb.fullPathSv().eq("dir/"_sv));
    CT_CHECK(pb.dirPartSv().eq("dir/"_sv));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    return 0;
}

constexpr i32 staticPathBuilderSetDirPartFlowTest() {
    core::StaticPathBuilder<128> pb = {};

    pb.setDirPart("testing/this"_sv);
    CT_CHECK(pb.fullPathSv().eq("testing/this/"_sv));
    CT_CHECK(pb.dirPartSv().eq("testing/this/"_sv));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.reset();
    pb.setDirPart("testing/this/"_sv);
    CT_CHECK(pb.fullPathSv().eq("testing/this/"_sv));
    CT_CHECK(pb.dirPartSv().eq("testing/this/"_sv));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.reset();
    pb.setDirPart("test"_sv);
    CT_CHECK(pb.fullPathSv().eq("test/"_sv));
    CT_CHECK(pb.dirPartSv().eq("test/"_sv));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.reset();
    pb.setDirPart(""_sv);
    CT_CHECK(pb.fullPathSv().eq(core::sv("")));
    CT_CHECK(pb.dirPartSv().eq(core::sv()));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.reset();
    pb.setDirPart("/"_sv);
    CT_CHECK(pb.fullPathSv().eq("/"_sv));
    CT_CHECK(pb.dirPartSv().eq("/"_sv));
    CT_CHECK(pb.filePartSv().eq(core::sv("")));
    CT_CHECK(pb.extPartSv().eq(core::sv()));

    pb.reset();
    pb.setDirPart("base"_sv);
    pb.setFilePart("file.txt"_sv);
    pb.appendToDirPath("child"_sv);
    CT_CHECK(pb.fullPathSv().eq("base/child/file.txt"_sv));
    CT_CHECK(pb.dirPartSv().eq("base/child/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.txt"_sv));
    CT_CHECK(pb.extPartSv().eq("txt"_sv));

    pb.reset();
    pb.setFilePart("file.txt"_sv);
    pb.appendToDirPath("child"_sv);
    CT_CHECK(pb.fullPathSv().eq("child/file.txt"_sv));
    CT_CHECK(pb.dirPartSv().eq("child/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.txt"_sv));
    CT_CHECK(pb.extPartSv().eq("txt"_sv));

    pb.reset();
    pb.setDirPart("/"_sv);
    pb.setFilePart("file.txt"_sv);
    pb.appendToDirPath("child"_sv);
    CT_CHECK(pb.fullPathSv().eq("/child/file.txt"_sv));
    CT_CHECK(pb.dirPartSv().eq("/child/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.txt"_sv));
    CT_CHECK(pb.extPartSv().eq("txt"_sv));

    pb.reset();
    pb.setFilePart("file.ext"_sv);
    pb.setDirPart("dir"_sv);
    CT_CHECK(pb.fullPathSv().eq("dir/file.ext"_sv));
    CT_CHECK(pb.dirPartSv().eq("dir/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.ext"_sv));
    CT_CHECK(pb.extPartSv().eq("ext"_sv));

    pb.setDirPart("much/longer/dir"_sv);
    CT_CHECK(pb.fullPathSv().eq("much/longer/dir/file.ext"_sv));
    CT_CHECK(pb.dirPartSv().eq("much/longer/dir/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.ext"_sv));
    CT_CHECK(pb.extPartSv().eq("ext"_sv));

    pb.setDirPart("d"_sv);
    CT_CHECK(pb.fullPathSv().eq("d/file.ext"_sv));
    CT_CHECK(pb.dirPartSv().eq("d/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.ext"_sv));
    CT_CHECK(pb.extPartSv().eq("ext"_sv));

    pb.setDirPart("/"_sv);
    CT_CHECK(pb.fullPathSv().eq("/file.ext"_sv));
    CT_CHECK(pb.dirPartSv().eq("/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.ext"_sv));
    CT_CHECK(pb.extPartSv().eq("ext"_sv));

    pb.setDirPart(""_sv); // TODO: This should set the dir path to "" instead of doing nothing
    CT_CHECK(pb.fullPathSv().eq("/file.ext"_sv));
    CT_CHECK(pb.dirPartSv().eq("/"_sv));
    CT_CHECK(pb.filePartSv().eq("file.ext"_sv));
    CT_CHECK(pb.extPartSv().eq("ext"_sv));

    return 0;
}

constexpr i32 staticPathBuilderPartsTest() {
    struct TestCase {
        const char* input;
        core::StrView expectedFull;
        core::StrView expectedDir;
        core::StrView expectedFile;
        core::StrView expectedExt;
    };

    constexpr TestCase cases[] = {
        { "t/tt.txt",              "t/tt.txt"_sv,              "t/"_sv,            "tt.txt"_sv,   "txt"_sv, },
        { "tt/tt/asd.cs",          "tt/tt/asd.cs"_sv,          "tt/tt/"_sv,        "asd.cs"_sv,   "cs"_sv,  },
        { "t/",                    "t/"_sv,                    "t/"_sv,            {},            {},       },
        { "noext",                 "noext"_sv,                 {},                 "noext"_sv,    {},       },
        { "a.",                    "a."_sv,                    {},                 "a."_sv,       {},       },
        { "/123456",               "/123456"_sv,               "/"_sv,             "123456"_sv,   {},       },
        { "dir.with.dot/file",     "dir.with.dot/file"_sv,     "dir.with.dot/"_sv, "file"_sv,     {},       },
        { "dir.with.dot/file.txt", "dir.with.dot/file.txt"_sv, "dir.with.dot/"_sv, "file.txt"_sv, "txt"_sv, },
        { "/",                     "/"_sv,                     "/"_sv,             {},            {},       },
        { nullptr,                 {},                          {},                {},            {},       },
    };

    i32 ret = core::testing::executeTestTable("StaticPathBuilder parts test failed at: ", cases, [](const auto& tc, const char* cErr) {
            core::StaticPathBuilder<128> pb = {};
            pb.setFilePart(core::sv(tc.input));

            core::StrView full = pb.fullPathSv();
            core::StrView dir = pb.dirPartSv();
            core::StrView file = pb.filePartSv();
            core::StrView ext = pb.extPartSv();

            CT_CHECK(full.eq(tc.expectedFull), cErr);
            CT_CHECK(dir.eq(tc.expectedDir), cErr);
            CT_CHECK(file.eq(tc.expectedFile), cErr);
            CT_CHECK(ext.eq(tc.expectedExt), cErr);

            if (tc.expectedDir.data() == nullptr) {
                CT_CHECK(dir.data() == nullptr, cErr);
            }

            return 0;
        }
    );
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 staticPathBuilderSetExtPartTest() {
    struct TestCase {
        const char* input;
        const char* newExt;
        core::StrView expectedFull;
        core::StrView expectedDir;
        core::StrView expectedFile;
        core::StrView expectedExt;
    };

    constexpr TestCase cases[] = {
        {
            .input = "t/tt.txt",
            .newExt = "bin",
            .expectedFull = core::sv("t/tt.bin"),
            .expectedDir = core::sv("t/"),
            .expectedFile = core::sv("tt.bin"),
            .expectedExt = core::sv("bin")
        },
        {
            .input = "tt/tt/asd.cs",
            .newExt = "h",
            .expectedFull = core::sv("tt/tt/asd.h"),
            .expectedDir = core::sv("tt/tt/"),
            .expectedFile = core::sv("asd.h"),
            .expectedExt = core::sv("h")
        },
        {
            .input = "a.",
            .newExt = "jpg",
            .expectedFull = core::sv("a.jpg"),
            .expectedDir = core::sv(),
            .expectedFile = core::sv("a.jpg"),
            .expectedExt = core::sv("jpg")
        },
        {
            .input = "dir.with.dot/file.txt",
            .newExt = "png",
            .expectedFull = core::sv("dir.with.dot/file.png"),
            .expectedDir = core::sv("dir.with.dot/"),
            .expectedFile = core::sv("file.png"),
            .expectedExt = core::sv("png")
        },
    };

    i32 ret = core::testing::executeTestTable("StaticPathBuilder setExtPart test failed at: ", cases, [](const auto& tc, const char* cErr) {
        core::StaticPathBuilder<128> pb = {};
        pb.setFilePart(core::sv(tc.input));
        pb.setExtPart(core::sv(tc.newExt));

        core::StrView full = pb.fullPathSv();
        core::StrView dir = pb.dirPartSv();
        core::StrView file = pb.filePartSv();
        core::StrView ext = pb.extPartSv();

        CT_CHECK(full.eq(tc.expectedFull), cErr);
        CT_CHECK(dir.eq(tc.expectedDir), cErr);
        CT_CHECK(file.eq(tc.expectedFile), cErr);
        CT_CHECK(ext.eq(tc.expectedExt), cErr);
        CT_CHECK(pb.extPart() != nullptr, cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 staticPathBuilderDirOpsTest() {
    struct TestCase {
        const char* dir;
        const char* append;
        const char* file;
        core::StrView expectedFull;
        core::StrView expectedDir;
        core::StrView expectedFile;
    };

    constexpr TestCase cases[] = {
        {
            .dir = "root",
            .append = nullptr,
            .file = "file.txt",
            .expectedFull = core::sv("root/file.txt"),
            .expectedDir = core::sv("root/"),
            .expectedFile = core::sv("file.txt")
        },
        {
            .dir = "root",
            .append = "child",
            .file = "file.txt",
            .expectedFull = core::sv("root/child/file.txt"),
            .expectedDir = core::sv("root/child/"),
            .expectedFile = core::sv("file.txt")
        },
        {
            .dir = nullptr,
            .append = "root",
            .file = "file.txt",
            .expectedFull = core::sv("root/file.txt"),
            .expectedDir = core::sv("root/"),
            .expectedFile = core::sv("file.txt")
        },
        {
            .dir = nullptr,
            .append = "root/child",
            .file = "file.txt",
            .expectedFull = core::sv("root/child/file.txt"),
            .expectedDir = core::sv("root/child/"),
            .expectedFile = core::sv("file.txt")
        },
        {
            .dir = nullptr,
            .append = "root",
            .file = nullptr,
            .expectedFull = core::sv("root/"),
            .expectedDir = core::sv("root/"),
            .expectedFile = core::sv()
        },
    };

    i32 ret = core::testing::executeTestTable("StaticPathBuilder dir ops test failed at: ", cases, [](const auto& tc, const char* cErr) {
        core::StaticPathBuilder<128> pb = {};
        pb.setDirPart(core::sv(tc.dir));
        pb.appendToDirPath(core::sv(tc.append));
        pb.setFilePart(core::sv(tc.file));

        core::StrView full = pb.fullPathSv();
        core::StrView dir = pb.dirPartSv();
        core::StrView file = pb.filePartSv();

        CT_CHECK(full.eq(tc.expectedFull), cErr);
        CT_CHECK(dir.eq(tc.expectedDir), cErr);
        CT_CHECK(file.eq(tc.expectedFile), cErr);

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 staticPathBuilderResetFilePartTest() {
    struct TestCase {
        const char* dir;
        const char* file;
        core::StrView expectedFull;
        core::StrView expectedDir;
    };

    constexpr TestCase cases[] = {
        {
            .dir = "root",
            .file = "file.txt",
            .expectedFull = core::sv("root/"),
            .expectedDir = core::sv("root/")
        },
        {
            .dir = "dir.with.dot",
            .file = "file.ext",
            .expectedFull = core::sv("dir.with.dot/"),
            .expectedDir = core::sv("dir.with.dot/")
        },
    };

    i32 ret = core::testing::executeTestTable("StaticPathBuilder resetFilePart test failed at: ", cases,
        [](const auto& tc, const char* cErr) {
            core::StaticPathBuilder<128> pb = {};
            pb.setDirPart(core::sv(tc.dir));
            pb.setFilePart(core::sv(tc.file));
            pb.resetFilePart();

            core::StrView full = pb.fullPathSv();
            core::StrView dir = pb.dirPartSv();
            core::StrView file = pb.filePartSv();
            core::StrView ext = pb.extPartSv();

            CT_CHECK(full.eq(tc.expectedFull), cErr);
            CT_CHECK(dir.eq(tc.expectedDir), cErr);
            CT_CHECK(file.eq(core::sv("")), cErr);
            CT_CHECK(ext.eq(core::sv()), cErr);
            CT_CHECK(pb.extPart() == nullptr, cErr);

            return 0;
        });
    CT_CHECK(ret == 0);

    return 0;
}

constexpr i32 staticPathBuilderResetExtPartTest() {
    struct TestCase {
        const char* input;
        core::StrView expectedFull;
        core::StrView expectedDir;
        core::StrView expectedFile;
        core::StrView expectedExt;
    };

    constexpr TestCase cases[] = {
        { "file.ext",               "file"_sv,               {},                 "file"_sv,       {},          },
        { "file",                   "file"_sv,               {},                 "file"_sv,       {},          },
        { "file.",                  "file"_sv,               {},                 "file"_sv,       {},          },
        { ".ext",                   {},                      {},                 {},              {},          },
        { "dir.with.dot/file.txt",  "dir.with.dot/file"_sv,  "dir.with.dot/"_sv, "file"_sv,       {},          },
        { "dir/file.tar.gz",        "dir/file.tar"_sv,       "dir/"_sv,          "file.tar"_sv,   "tar"_sv,    },
        { "dir/.ext",               "dir/"_sv,               "dir/"_sv,          {},              {},          },
        { "dir/.hidden.ext",        "dir/.hidden"_sv,        "dir/"_sv,          ".hidden"_sv,    "hidden"_sv, },
        { "dir/",                   "dir/"_sv,              "dir/"_sv,           {},              {},          },
        { nullptr,                  {},                      {},                 {},              {},          },
    };

    i32 ret = core::testing::executeTestTable("StaticPathBuilder resetExtPart test failed at: ", cases,
        [](const auto& tc, const char* cErr) {
            core::StaticPathBuilder<128> pb = {};
            pb.setFilePart(core::sv(tc.input));
            pb.resetExtPart();

            core::StrView full = pb.fullPathSv();
            core::StrView dir = pb.dirPartSv();
            core::StrView file = pb.filePartSv();
            core::StrView ext = pb.extPartSv();

            CT_CHECK(full.eq(tc.expectedFull), cErr);
            CT_CHECK(dir.eq(tc.expectedDir), cErr);
            CT_CHECK(file.eq(tc.expectedFile), cErr);
            CT_CHECK(ext.eq(tc.expectedExt), cErr);

            return 0;
        });
    CT_CHECK(ret == 0);

    return 0;
}

} // namespace

i32 runPltPathTestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo(sInfo);

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderBasicFlowTest);
    if (runTest(tInfo, staticPathBuilderBasicFlowTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderSetExtPartFlowTest);
    if (runTest(tInfo, staticPathBuilderSetExtPartFlowTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderSetFilePartFlowTest);
    if (runTest(tInfo, staticPathBuilderSetFilePartFlowTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderSetDirPartFlowTest);
    if (runTest(tInfo, staticPathBuilderSetDirPartFlowTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderPartsTest);
    if (runTest(tInfo, staticPathBuilderPartsTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderSetExtPartTest);
    if (runTest(tInfo, staticPathBuilderSetExtPartTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderDirOpsTest);
    if (runTest(tInfo, staticPathBuilderDirOpsTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderResetFilePartTest);
    if (runTest(tInfo, staticPathBuilderResetFilePartTest) != 0) { ret = -1; }

    tInfo.name = FN_NAME_TO_CPTR(staticPathBuilderResetExtPartTest);
    if (runTest(tInfo, staticPathBuilderResetExtPartTest) != 0) { ret = -1; }

    return ret;
}

constexpr i32 runCompiletimePltPathTestsSuite() {
    RunTestCompileTime(staticPathBuilderBasicFlowTest);
    RunTestCompileTime(staticPathBuilderSetExtPartFlowTest);
    RunTestCompileTime(staticPathBuilderSetFilePartFlowTest);
    RunTestCompileTime(staticPathBuilderSetDirPartFlowTest);
    RunTestCompileTime(staticPathBuilderPartsTest);
    RunTestCompileTime(staticPathBuilderSetExtPartTest);
    RunTestCompileTime(staticPathBuilderDirOpsTest);
    RunTestCompileTime(staticPathBuilderResetFilePartTest);
    RunTestCompileTime(staticPathBuilderResetExtPartTest);

    return 0;
}
