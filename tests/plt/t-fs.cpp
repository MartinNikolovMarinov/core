#include "../t-index.h"
#include "core_exec_ctx.h"
#include "plt/core_fs.h"

namespace {

constexpr const char* testDirectory = PATH_TO_TEST_DATA "/testing_directory";

constexpr const char* testNamesTable[] = {
    "File Name With Spaces.txt",
    "VeryVeryLongFileNameThatExceedsNormalLengthsAndKeepsGoingAndGoingAndGoingSeeIfThisWorks.txt",

    "测试文件.txt",                        // (Chinese characters)
    "テストファイル.txt",                   // (Japanese characters)
    "테스트파일.txt",                       // (Korean characters)
    "ПримеренФайл.txt",                  // (Cyrillic characters - Bulgarian)
    "ΔοκιμαστικόΑρχείο.txt",             // (Greek characters)
    "Prüfungdatei.txt",                  // (German Umlaut)
    "FichierÉpreuve.txt",                // (French accented characters)
    "ArquivoDeTeste.txt",                // (Portuguese characters)
    "ArchivoDePrueba.txt",               // (Spanish characters)
    "टेस्टफाइल.txt",                        // (Hindi characters)
    "קובץבדיקה.txt",                       // (Hebrew characters)
    "ملفالاختبار.txt",                    // (Arabic characters)
    "🚀Rocket🌟Star.txt",                // (Emojis)
    "File&Name@Special#Char$.txt",       // (Special characters)
    "🎵MusicNote♫.txt",                  // (Musical symbols)
    "🧪💡ExperimentLightBulb.txt",       // (Multiple emojis)
    "🐱‍👓HackerCat.txt",                 // (Compound emoji)
    "ƒîlèNäméŵîthŠpecîålČhåråčterš.txt", // (Mixed accented characters)
};

[[nodiscard]]
i32 closeAndDeleteFile(core::FileDesc&& fd, const char* path) {
    {
        auto res = core::fileClose(fd);
        CT_CHECK(!res.hasErr(), "File closing failed");
    }
    {
        auto res = core::fileDelete(path);
        CT_CHECK(!res.hasErr(), "File deletion failed");
    }
    return 0;
}

using TestPathBuilder = core::StaticPathBuilder<256>;

[[nodiscard]]
i32 tryOpenFileWithMostCommonModeCombinations(const char* path) {
    //  With Default Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Read Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Read);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Write Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Write);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Read and Write Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Read | core::OpenMode::Write);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Append Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Append);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Append and Read Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Append | core::OpenMode::Read);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Append and Write Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Append | core::OpenMode::Write);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Create Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Create);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // With Truncate Mode:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Truncate);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    // All possible:
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(path, core::OpenMode::Read |
                                            core::OpenMode::Write |
                                            core::OpenMode::Append |
                                            core::OpenMode::Create |
                                            core::OpenMode::Truncate);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
            CT_CHECK(f.isValid());
        }
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
            CT_CHECK(!f.isValid());
        }
    }

    return 0;
}

bool createTestDirecotry() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);

    auto res = core::dirCreate(pb.fullPath());
    if (res.hasErr()) {
        return false;
    }

    return true;
}

bool checkTestDirecotryIsCleanned() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);

    auto res = core::dirIsEmpty(pb.fullPath());
    if (res.hasErr()) {
        return false;
    }

    bool result = res.value();
    return result;
}

// ------------------------------- Tests start from here ---------------------------------------------------------------

i32 createAndDeleteFileTest() {
    core::FileDesc f;
    TestPathBuilder pb = {};

    core::memset(pb.buff, char(1), 256);

    pb.setDirPart(testDirectory);
    pb.setFilePart("test.txt");

    {
        auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Create);
        CT_CHECK(!res.hasErr(), "File creation failed");
        f = std::move(res.value());
    }

    CT_CHECK(closeAndDeleteFile(std::move(f), pb.fullPath()) == 0);

    return 0;
}

i32 createFilesAndCheckIfTheyExistTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testNamesTable, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFilePart(c);

        core::FileDesc f;

        // Open with create
        {
            auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Create);
            CT_CHECK(!res.hasErr(), cErr);
            f = std::move(res.value());
        }

        // Close
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr(), cErr);
        }

        CT_CHECK(tryOpenFileWithMostCommonModeCombinations(pb.fullPath()) == 0);

        // Delete
        {
            auto res = core::fileDelete(pb.fullPath());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 checkFileStatsTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testNamesTable, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFilePart(c);

        core::FileDesc f;

        // Open with create
        {
            auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Create);
            CT_CHECK(!res.hasErr(), cErr);
            f = std::move(res.value());
        }

        // Size
        {
            auto res = core::fileSize(f);
            CT_CHECK(!res.hasErr(), cErr);
            CT_CHECK(res.value() == 0, cErr);
        }

        // Close
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr(), cErr);
        }

        // Stat
        {
            core::FileStat stat;
            auto res = core::fileStat(pb.fullPath(), stat);
            CT_CHECK(!res.hasErr(), cErr);
            CT_CHECK(stat.size == 0, cErr);
            CT_CHECK(stat.type == core::FileType::Regular, cErr);
        }

        // Delete
        {
            auto res = core::fileDelete(pb.fullPath());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 fileCopyTest() {
    TestPathBuilder pbSrc;
    pbSrc.setDirPart(testDirectory);
    pbSrc.setFilePart("copy_src.txt");
    const char* srcPath = pbSrc.fullPath();

    TestPathBuilder pbDst;
    pbDst.setDirPart(testDirectory);
    pbDst.setFilePart("copy_dst.txt");
    const char* dstPath = pbDst.fullPath();

    constexpr const char* payload = "copy payload";

    // Create and write source file
    {
        core::FileDesc f;
        auto res = core::fileOpen(srcPath, core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create | core::OpenMode::Truncate);
        CT_CHECK(!res.hasErr());
        f = std::move(res.value());

        auto w = core::fileWrite(f, payload, core::cstrLen(payload));
        CT_CHECK(!w.hasErr());
        auto closeRes = core::fileClose(f);
        CT_CHECK(!closeRes.hasErr());
    }

    // Copy to destination
    {
        auto res = core::fileCopy(srcPath, dstPath);
        CT_CHECK(!res.hasErr());
    }

    // Validate destination contents and size
    {
        core::FileDesc f;
        auto res = core::fileOpen(dstPath, core::OpenMode::Read);
        CT_CHECK(!res.hasErr());
        f = std::move(res.value());

        core::FileStat st{};
        auto statRes = core::fileStat(f, st);
        CT_CHECK(!statRes.hasErr());
        CT_CHECK(st.type == core::FileType::Regular);
        CT_CHECK(st.size == core::cstrLen(payload));

        char buff[32] = {};
        auto readRes = core::fileRead(f, buff, 32);
        CT_CHECK(!readRes.hasErr());
        CT_CHECK(readRes.value() == core::cstrLen(payload));
        CT_CHECK(core::memcmp(buff, readRes.value(), payload, core::cstrLen(payload)) == 0);

        auto closeRes = core::fileClose(f);
        CT_CHECK(!closeRes.hasErr());
    }

    // Source should remain intact
    {
        core::FileStat st{};
        auto statRes = core::fileStat(srcPath, st);
        CT_CHECK(!statRes.hasErr());
        CT_CHECK(st.size == core::cstrLen(payload));
    }

    // Cleanup
    {
        auto res = core::fileDelete(srcPath);
        CT_CHECK(!res.hasErr());
    }
    {
        auto res = core::fileDelete(dstPath);
        CT_CHECK(!res.hasErr());
    }

    return 0;
}

i32 fileTruncateAndStatTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);

    // Truncate via descriptor.
    pb.setFilePart("truncate_fd.txt");

    core::FileDesc fdTruncate;
    {
        auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create | core::OpenMode::Truncate);
        CT_CHECK(!res.hasErr());
        fdTruncate = std::move(res.value());
    }
    {
        constexpr const char* data = "abcdef";
        auto res = core::fileWrite(fdTruncate, data, core::cstrLen(data));
        CT_CHECK(!res.hasErr());
    }
    {
        auto res = core::fileTruncate(fdTruncate, 3);
        CT_CHECK(!res.hasErr());
    }
    {
        core::FileStat stat{};
        auto res = core::fileStat(fdTruncate, stat);
        CT_CHECK(!res.hasErr());
        CT_CHECK(stat.type == core::FileType::Regular);
        CT_CHECK(stat.size == 3);
    }
    {
        auto res = core::fileSeek(fdTruncate, 0, core::SeekMode::Begin);
        CT_CHECK(!res.hasErr());

        char buff[8] = {};
        auto res2 = core::fileRead(fdTruncate, buff, 6);
        CT_CHECK(!res2.hasErr());
        CT_CHECK(res2.value() == 3);
        CT_CHECK(core::memcmp(buff, res2.value(), "abc", 3) == 0);
    }
    CT_CHECK(closeAndDeleteFile(std::move(fdTruncate), pb.fullPath()) == 0);

    // Truncate via path (shrink then extend).
    pb.setFilePart("truncate_path.txt");
    {
        auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create | core::OpenMode::Truncate);
        CT_CHECK(!res.hasErr());

        core::FileDesc f = std::move(res.value());
        auto wr = core::fileWrite(f, "123456", 6);
        CT_CHECK(!wr.hasErr());
        auto closeRes = core::fileClose(f);
        CT_CHECK(!closeRes.hasErr());
    }
    {
        auto res = core::fileTruncate(pb.fullPath(), 4);
        CT_CHECK(!res.hasErr());
        core::FileStat stat{};
        auto statRes = core::fileStat(pb.fullPath(), stat);
        CT_CHECK(!statRes.hasErr());
        CT_CHECK(stat.size == 4);
    }
    {
        auto res = core::fileTruncate(pb.fullPath(), 10);
        CT_CHECK(!res.hasErr());
        core::FileStat stat{};
        auto statRes = core::fileStat(pb.fullPath(), stat);
        CT_CHECK(!statRes.hasErr());
        CT_CHECK(stat.size == 10);
    }
    {
        auto res = core::fileDelete(pb.fullPath());
        CT_CHECK(!res.hasErr());
    }

    return 0;
}

i32 fileFlushTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);
    pb.setFilePart("flush_test.txt");

    core::FileDesc writer;
    {
        auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create | core::OpenMode::Truncate);
        CT_CHECK(!res.hasErr());
        writer = std::move(res.value());
    }
    {
        constexpr const char* payload = "flushed!";
        auto res = core::fileWrite(writer, payload, core::cstrLen(payload));
        CT_CHECK(!res.hasErr());
    }
    {
        auto res = core::fileFlush(writer);
        CT_CHECK(!res.hasErr());
    }

    core::FileDesc reader;
    {
        auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Read);
        CT_CHECK(!res.hasErr());
        reader = std::move(res.value());
    }
    {
        char buff[16] = {};
        auto res = core::fileRead(reader, buff, 16);
        CT_CHECK(!res.hasErr());
        CT_CHECK(res.value() == core::cstrLen("flushed!"));
        CT_CHECK(core::memcmp(buff, res.value(), "flushed!", core::cstrLen("flushed!")) == 0);
    }
    {
        auto res = core::fileClose(reader);
        CT_CHECK(!res.hasErr());
    }

    CT_CHECK(closeAndDeleteFile(std::move(writer), pb.fullPath()) == 0);

    return 0;
}

template <core::AllocatorId TAllocId>
i32 commonErrorsTest() {

    // Test operations on non-existent files
    {
        struct TestCase {
            const char* path;
            core::OpenMode mode;
        };

        TestCase testCases[] = {
            {nullptr, core::OpenMode::Default},
            {"./does/not/exist and can't create 1", core::OpenMode::Create},
        };

        for (auto& tc : testCases) {
            {
                auto res = core::fileOpen(tc.path, tc.mode);
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::fileDelete(tc.path);
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::fileMove(tc.path, "new name");
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::fileCopy(tc.path, "new name");
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::fileTruncate(tc.path, 5);
                CT_CHECK(res.hasErr());
            }
            {
                [[maybe_unused]] core::FileStat unused;
                auto res = core::fileStat(tc.path, unused);
                CT_CHECK(res.hasErr());
            }
            {
                [[maybe_unused]] core::Memory<u8> unused;
                auto res = core::fileReadEntire(tc.path, unused);
                CT_CHECK(res.hasErr());
            }
            {
                [[maybe_unused]] core::ArrList<u8> unused(5, 0);
                auto res = core::fileWriteEntire(tc.path, unused.data(), unused.len());
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::dirCreate(tc.path);
                CT_CHECK(res.hasErr());
            }
            {
                core::DirWalkCallback cb = [](const core::DirEntry&, addr_size, void*) -> bool { return true; };
                auto res = core::dirWalk(tc.path, cb, nullptr);
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::dirDelete(tc.path);
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::dirDeleteRec<TAllocId>(tc.path);
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::dirChangeCWD(tc.path);
                CT_CHECK(res.hasErr());
            }
        }
    }

    // dirCWD invalid buffer or size
    {
        auto res = core::dirCWD(nullptr, 0);
        CT_CHECK(res.hasErr());

        char tiny[1] = {};
        auto res2 = core::dirCWD(tiny, 0);
        CT_CHECK(res2.hasErr());
    }

    // Test operations on invalid file descriptor
    {
        auto check = [](const auto& r) {
            CT_CHECK(r.hasErr());
            CT_CHECK(r.err() == core::ERR_PASSED_INVALID_FILE_DESCRIPTOR);
            return 0;
        };

        core::FileDesc f;
        CT_CHECK(check(core::fileClose(f)) == 0);
        CT_CHECK(check(core::fileWrite(f, nullptr, 0)) == 0);
        CT_CHECK(check(core::fileRead(f, nullptr, 0)) == 0);
        CT_CHECK(check(core::fileSeek(f, 0)) == 0);
        CT_CHECK(check(core::fileSize(f)) == 0);
        CT_CHECK(check(core::fileFlush(f)) == 0);
        CT_CHECK(check(core::fileTruncate(f, 0)) == 0);
    }

    return 0;
}

i32 edgeErrorCasesTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);
    pb.setFilePart("test.txt");

    // Files opened for reading should fail to write
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Read | core::OpenMode::Create);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
        }
        {
            auto res = core::fileWrite(f, "should fail", core::cstrLen("should fail"));
            CT_CHECK(res.hasErr());
        }
        CT_CHECK(closeAndDeleteFile(std::move(f), pb.fullPath()) == 0);
    }

    // Files oppned for writing should fail to read
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Write | core::OpenMode::Create);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
        }
        {
            char buff[256] = {};
            auto res = core::fileRead(f, buff, 256);
            CT_CHECK(res.hasErr());
        }
        CT_CHECK(closeAndDeleteFile(std::move(f), pb.fullPath()) == 0);
    }

    // Double closing a file
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Create);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
        }

        void* ptrToRawHandle = f.handle;

        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr());
        }
        {
            f.handle = ptrToRawHandle; // This is completely invalid usecase, but the API should still handle it.
            auto res = core::fileClose(f);
            CT_CHECK(res.hasErr(), "closing a closed file should fail");
        }
    }

    // Delete the file
    {
        {
            auto res = core::fileDelete(pb.fullPath());
            CT_CHECK(!res.hasErr());
        }
        {
            auto res = core::fileDelete(pb.fullPath()); // No crashes on double delete.
            CT_CHECK(res.hasErr());
        }
    }

    // Using a nullptr buffer with a valid descriptor.
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Read | core::OpenMode::Write | core::OpenMode::Create);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
        }
        {
            auto res = core::fileWrite(f, nullptr, 50);
            CT_CHECK(res.hasErr());
        }
        {
            auto res = core::fileRead(f, nullptr, 50);
            CT_CHECK(res.hasErr());
        }

        CT_CHECK(closeAndDeleteFile(std::move(f), pb.fullPath()) == 0);
    }

    return 0;
}

i32 dirCwdChangeTest() {
    constexpr addr_size BUF_SIZE = 512;
    char initial[BUF_SIZE] = {};
    {
        auto res = core::dirCWD(initial, BUF_SIZE);
        CT_CHECK(!res.hasErr());
    }

    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);
    pb.appendToDirPath("cwd_test");

    {
        auto res = core::dirCreate(pb.fullPath());
        CT_CHECK(!res.hasErr());
    }

    {
        auto res = core::dirChangeCWD(pb.fullPath());
        CT_CHECK(!res.hasErr());
    }
    {
        char after[BUF_SIZE] = {};
        auto res = core::dirCWD(after, BUF_SIZE);
        CT_CHECK(!res.hasErr());
        CT_CHECK(core::memcmp(after, core::cstrLen(after), pb.fullPath(), core::cstrLen(pb.fullPath()) - 1) == 0);
    }

    {
        auto res = core::dirChangeCWD(initial);
        CT_CHECK(!res.hasErr());
    }
    {
        char back[BUF_SIZE] = {};
        auto res = core::dirCWD(back, BUF_SIZE);
        CT_CHECK(!res.hasErr());
        CT_CHECK(core::memcmp(back, core::cstrLen(back), initial, core::cstrLen(initial)) == 0);
    }

    {
        auto res = core::dirDelete(pb.fullPath());
        CT_CHECK(!res.hasErr());
    }

    return 0;
}

i32 directoriesCreateMoveAndDeleteTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testNamesTable, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFilePart(c);

        // Create directory
        {
            auto res = core::dirCreate(pb.fullPath());
            CT_CHECK(!res.hasErr(), cErr);
        }

        TestPathBuilder moveddpb;
        moveddpb.setDirPart(testDirectory);
        moveddpb.setFilePart("moved");

        // Move It
        {
            auto res = core::fileMove(pb.fullPath(), moveddpb.fullPath());
            CT_CHECK(!res.hasErr(), cErr);
        }

        // Stat It
        {
            core::FileStat stat;
            auto res = core::fileStat(moveddpb.fullPath(), stat);
            CT_CHECK(!res.hasErr(), cErr);
            CT_CHECK(stat.type == core::FileType::Directory, cErr);
        }

        // Delete It
        {
            auto res = core::dirDelete(moveddpb.fullPath());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 mostBasicReadAndWriteTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);
    pb.setFilePart("test.txt");

    core::FileDesc f;
    {
        auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create);
        CT_CHECK(!res.hasErr());
        f = std::move(res.value());
    }

    // Write "hello"
    {
        auto res = core::fileWrite(f, "hello", 5);
        CT_CHECK(!res.hasErr());
        CT_CHECK(res.value() == 5);
    }

    // Seek to beginning
    {
        auto res = core::fileSeek(f, 0, core::SeekMode::Begin);
        CT_CHECK(!res.hasErr());
        CT_CHECK(res.value() == 0, "Offset should be 0 after seeking to beginning");
    }

    // Read "hello"
    {
        char buff[6] = {};
        auto res = core::fileRead(f, buff, 5);
        CT_CHECK(!res.hasErr());
        CT_CHECK(res.value() == 5);
        CT_CHECK(core::memcmp(buff, core::cstrLen(buff), "hello", 5) == 0);
    }

    CT_CHECK(closeAndDeleteFile(std::move(f), pb.fullPath()) == 0);

    return 0;
}

template <core::AllocatorId TAllocId>
i32 basicListDirectoryContentsTest() {
    TestPathBuilder pb = {};

    pb.setDirPart(testDirectory);
    pb.appendToDirPath("test_directory");

    const char* basicFileNames[] = {
        "test0.txt",
        "test1.txt",
        "test2.txt",
    };
    constexpr addr_size basicFileNamesLen = CORE_C_ARRLEN(basicFileNames);

    const char* basicDirNames[] = {
        "test_sub_directory_0",
        "test_sub_directory_1",
    };
    constexpr addr_size basicDirNamesLen = CORE_C_ARRLEN(basicDirNames);

    // Create directory
    {
        auto res = core::dirCreate(pb.fullPath());
        CT_CHECK(!res.hasErr());
    }

    // Create the test files inside the directory
    {
        for (addr_size i = 0; i < basicFileNamesLen; ++i) {
            pb.resetFilePart();
            pb.setFilePart(basicFileNames[i]);

            core::FileDesc f;
            {
                auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Create);
                CT_CHECK(!res.hasErr());
                f = std::move(res.value());
            }
            {
                auto res = core::fileClose(f);
                CT_CHECK(!res.hasErr());
            }
        }

        for (addr_size i = 0; i < basicDirNamesLen; ++i) {
            pb.resetFilePart();
            pb.setFilePart(basicDirNames[i]);

            auto res = core::dirCreate(pb.fullPath());
            CT_CHECK(!res.hasErr());
        }

        pb.resetFilePart(); // reset to the test direcotry

        // Make sure the root test directory is no longer considered empty.
        {
            auto res = core::dirIsEmpty(pb.fullPath());
            CT_CHECK(!res.hasErr());
            CT_CHECK(res.value() == false);
        }
    }

    // List directory contents
    {
        struct Closure {
            const char** fileNames;
            addr_size fileNamesLen;
            const char** dirNames;
            addr_size dirNamesLen;

            addr_size& fileCount;
            addr_size& dirCount;
        };

        core::DirWalkCallback listWalk = [](const core::DirEntry& de, addr_size, void* userData) -> bool {
            Closure* c = reinterpret_cast<Closure*>(userData);

            const char** fileNames = c->fileNames;
            addr_size fileNamesLen = c->fileNamesLen;
            addr_size& fileCount = c->fileCount;

            const char** dirNames = c->dirNames;
            addr_size dirNamesLen = c->dirNamesLen;
            addr_size& dirCount = c->dirCount;

            const char* got = de.name;

            if (de.type == core::FileType::Regular) {
                addr_off foundIdx = core::find(fileNames, fileNamesLen, [&] (const char* elem, addr_size) -> bool {
                    bool ret = core::memcmp(elem, core::cstrLen(elem), got, core::cstrLen(got)) == 0;
                    return ret;
                });
                if (foundIdx == -1) return false;
                fileCount++;
            }
            else if (de.type == core::FileType::Directory) {
                addr_off foundIdx = core::find(dirNames, dirNamesLen, [&] (const char* elem, addr_size) -> bool {
                    bool ret = core::memcmp(elem, core::cstrLen(elem), got, core::cstrLen(got)) == 0;
                    return ret;
                });
                if (foundIdx == -1) return false;
                dirCount++;
            }
            else {
                return false;
            }

            return true;
        };

        addr_size fileCount = 0;
        addr_size dirCount = 0;
        Closure closure = {
            basicFileNames, basicFileNamesLen,
            basicDirNames, basicDirNamesLen,
            fileCount, dirCount
        };
        auto res = core::dirWalk(pb.fullPath(), listWalk,  reinterpret_cast<void*>(&closure));

        CT_CHECK(!res.hasErr());
        CT_CHECK(fileCount == basicFileNamesLen);
        CT_CHECK(dirCount == basicDirNamesLen);
    }

    // Delete directory
    {
        auto res = core::dirDeleteRec<core::DEFAULT_ALLOCATOR_ID>(pb.fullPath());
        CT_CHECK(!res.hasErr());
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 readAndWriteEntireFileTest() {
    struct TestCase {
        const char* path;
        const char* content;
    };

    TestCase testCases[] = {
        {"test0.txt", "123456789"},
        {"test1.txt", "123456789 1"},
        {"test1.txt", "123456789 12"},
        {"test2.txt", "123456789 123"},
        {"test3.txt", "123456789 1234"},
        {"test4.txt", "123456789 12345"},
        {"test5.txt", "123456789 123456"},
        {"test6.txt", "123456789 1234567"},
        {"test7.txt", "123456789 12345678"},
        {"test8.txt", "123456789 123456789"},
        {"test9.txt", "123456789 123456789"},

        {"test10.txt", "123456789 123456789 123456789"},
        {"test11.txt", "123456789 123456789 123456789 123456789"},
        {"test12.txt", "123456789 123456789 123456789 123456789 123456789"},
        {"test13.txt", "123456789 123456789 123456789 123456789 123456789 123456789"},
        {"test14.txt", "123456789 123456789 123456789 123456789 123456789 123456789 123456789"},
        {"test15.txt", "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789"}
    };

    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testCases, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFilePart(c.path);

        core::ArrList<u8, TAllocId> content(core::cstrLen(c.content));
        for (addr_size i = 0; i < core::cstrLen(c.content); ++i) {
            content.push(u8(c.content[i]));
        }

        // Write the file
        {
            auto res = core::fileWriteEntire(pb.fullPath(), content.data(), content.len());
            CT_CHECK(!res.hasErr(), cErr);
        }

        // Get file size
        addr_size fileSize = 0;
        u8* rawBuffer = nullptr;
        {
            core::FileStat fileStat;
            core::fileStat(pb.fullPath(), fileStat);
            fileSize = fileStat.size;
            rawBuffer = reinterpret_cast<u8*>(
                core::getAllocator(TAllocId).alloc(fileSize, sizeof(u8))
            );
        }
        defer {
            core::getAllocator(TAllocId).free(rawBuffer, fileSize, sizeof(u8));
        };

        // Read the file
        {
            core::Memory<u8> readBuffer (rawBuffer, fileSize);
            i32 counter = 5;
            while (counter-- > 0) {
                // Read the entire file a couple of times with the same buffer.
                auto res = core::fileReadEntire(pb.fullPath(), readBuffer);
                CT_CHECK(!res.hasErr(), cErr);
                CT_CHECK(content.len() == readBuffer.len(), cErr);
                for (addr_size i = 0; i < content.len(); ++i) {
                    CT_CHECK(content[i] == readBuffer[i], cErr);
                }
            }
        }

        // Delete the file
        {
            auto res = core::fileDelete(pb.fullPath());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 seekWriteAndReadTest() {
    TestPathBuilder pb = {};
    pb.setDirPart(testDirectory);
    pb.setFilePart("test.txt");

    core::FileDesc f;

    {
        auto res = core::fileOpen(pb.fullPath(), core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create);
        CT_CHECK(!res.hasErr());
        f = std::move(res.value());
    }

    {
        auto res = core::fileWrite(f, "000456000", 9);
        CT_CHECK(!res.hasErr());
    }

    {
        auto res = core::fileSeek(f, 0, core::SeekMode::Begin);
        CT_CHECK(!res.hasErr());

        auto res2 = core::fileWrite(f, "123", 3);
        CT_CHECK(!res2.hasErr());
    }

    {
        auto res = core::fileSeek(f, 3, core::SeekMode::Current);
        CT_CHECK(!res.hasErr());

        auto res2 = core::fileWrite(f, "78", 2);
        CT_CHECK(!res2.hasErr());
    }

    {
        auto res = core::fileSeek(f, -1, core::SeekMode::End);
        CT_CHECK(!res.hasErr());

        auto res2 = core::fileWrite(f, "9", 1);
        CT_CHECK(!res2.hasErr());
    }

    {
        auto res = core::fileSeek(f, 0, core::SeekMode::Begin);
        CT_CHECK(!res.hasErr());

        char buff[10] = {};
        auto res2 = core::fileRead(f, buff, 9);
        CT_CHECK(!res2.hasErr());
        CT_CHECK(core::memcmp(buff, core::cstrLen(buff), "123456789", 9) == 0);
    }

    CT_CHECK(closeAndDeleteFile(std::move(f), pb.fullPath()) == 0);

    return 0;
}

template <core::AllocatorId TAllocId>
i32 runDynamicMemoryTests(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo(TAllocId, sInfo.useAnsiColors, false);

    defer { core::getAllocator(TAllocId).clear(); };

    tInfo.name = FN_NAME_TO_CPTR(basicListDirectoryContentsTest);
    if (runTest(tInfo, basicListDirectoryContentsTest<TAllocId>) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(readAndWriteEntireFileTest);
    if (runTest(tInfo, readAndWriteEntireFileTest<TAllocId>) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(commonErrorsTest);
    if (runTest(tInfo, commonErrorsTest<TAllocId>) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    return 0;
}

} // namespace

i32 runPltFileSystemTestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    if (!createTestDirecotry()) {
        return -1;
    }

    TestInfo tInfo = createTestInfo(sInfo);

    // The tests below likely do allocate memory inside cstd but it's not tracked by registered allocators.
    tInfo.expectZeroAllocations = true;

    tInfo.name = FN_NAME_TO_CPTR(createAndDeleteFileTest);
    if (runTest(tInfo, createAndDeleteFileTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(createFilesAndCheckIfTheyExistTest);
    if (runTest(tInfo, createFilesAndCheckIfTheyExistTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(checkFileStatsTest);
    if (runTest(tInfo, checkFileStatsTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(fileCopyTest);
    if (runTest(tInfo, fileCopyTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(fileTruncateAndStatTest);
    if (runTest(tInfo, fileTruncateAndStatTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(fileFlushTest);
    if (runTest(tInfo, fileFlushTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(dirCwdChangeTest);
    if (runTest(tInfo, dirCwdChangeTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(directoriesCreateMoveAndDeleteTest);
    if (runTest(tInfo, directoriesCreateMoveAndDeleteTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(seekWriteAndReadTest);
    if (runTest(tInfo, seekWriteAndReadTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(mostBasicReadAndWriteTest);
    if (runTest(tInfo, mostBasicReadAndWriteTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(edgeErrorCasesTest);
    if (runTest(tInfo, edgeErrorCasesTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    // Below this point tests use dynamic memory

    if (runDynamicMemoryTests<RA_STD_ALLOCATOR_ID>(sInfo) != 0) { return -1; }
    if (runDynamicMemoryTests<RA_STD_STATS_ALLOCATOR_ID>(sInfo) != 0) { return -1; }
    if (runDynamicMemoryTests<RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID>(sInfo) != 0) { return -1; }
    if (runDynamicMemoryTests<RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID>(sInfo) != 0) { return -1; }

    constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE * 5;
    char buf[BUFFER_SIZE];
    setBufferForBumpAllocator(buf, BUFFER_SIZE);
    if (runDynamicMemoryTests<RA_BUMP_ALLOCATOR_ID>(sInfo) != 0) { return -1; }

    setBlockSizeForArenaAllocator(512);
    if (runDynamicMemoryTests<RA_ARENA_ALLOCATOR_ID>(sInfo) != 0) { return -1; }

    return 0;
}
