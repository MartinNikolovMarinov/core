#include "../t-index.h"

// FIXME: Valgrind reports about 50 problems for the test suite! Fix this at some point.

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

struct TestPathBuilder {
    static constexpr char FILE_SEPARATOR = '/';

    char buff[256];
    addr_size dirPathLen;

    TestPathBuilder() : buff{}, dirPathLen(0) {}

    void setDirPath(const char* dirPath) {
        dirPathLen = core::cstrLen(dirPath);
        core::memcopy(buff, dirPath, dirPathLen);
    }

    void appendToDirPath(const char* dirPath) {
        buff[dirPathLen] = FILE_SEPARATOR;
        dirPathLen += 1;
        addr_size len = core::cstrLen(dirPath);
        core::memcopy(buff + dirPathLen, dirPath, len);
        dirPathLen += len;
    }

    const char* fileName() const { return buff + dirPathLen + 1; }

    const char* path() const { return buff; }

    char* filePart() { return buff + dirPathLen; }

    addr_size filePartLen() { return core::cstrLen(filePart()); }

    void setFilePart(const char* fpath) {
        addr_size len = core::cstrLen(fpath);
        core::memcopy(filePart(), fpath, len);
        filePart()[len] = '\0';
    }

    void setFileName(const char* fname) {
        filePart()[0] = FILE_SEPARATOR;
        addr_size len = core::cstrLen(fname);
        core::memcopy(filePart() + 1, fname, len);
        filePart()[len + 1] = '\0';
    }

    void resetFilePart() {
        core::memset(filePart(), 0, filePartLen());
    }

    void reset() {
        core::memset(buff, 0, sizeof(buff));
        dirPathLen = 0;
    }
};

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
};

bool createTestDirecotry() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);

    auto res = core::dirCreate(pb.path());
    if (res.hasErr()) {
        return false;
    }

    return true;
}

bool checkTestDirecotryIsCleanned() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);

    auto res = core::dirIsEmpty(pb.path());
    if (res.hasErr()) {
        return false;
    }

    bool result = res.value();
    return result;
}

// ------------------------------- Tests start from here ---------------------------------------------------------------

i32 createAndDeleteFileTest() {
    core::FileDesc f;
    TestPathBuilder pb;

    core::memset(pb.buff, 1, 256);

    pb.setDirPath(testDirectory);
    pb.setFileName("test.txt");

    {
        auto res = core::fileOpen(pb.path(), core::OpenMode::Create);
        CT_CHECK(!res.hasErr(), "File creation failed");
        f = std::move(res.value());
    }

    CT_CHECK(closeAndDeleteFile(std::move(f), pb.path()) == 0);

    return 0;
}

i32 createFilesAndCheckIfTheyExistTest() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testNamesTable, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFileName(c);

        core::FileDesc f;

        // Open with create
        {
            auto res = core::fileOpen(pb.path(), core::OpenMode::Create);
            CT_CHECK(!res.hasErr(), cErr);
            f = std::move(res.value());
        }

        // Close
        {
            auto res = core::fileClose(f);
            CT_CHECK(!res.hasErr(), cErr);
        }

        CT_CHECK(tryOpenFileWithMostCommonModeCombinations(pb.path()) == 0);

        // Delete
        {
            auto res = core::fileDelete(pb.path());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 checkFileStatsTest() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testNamesTable, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFileName(c);

        core::FileDesc f;

        // Open with create
        {
            auto res = core::fileOpen(pb.path(), core::OpenMode::Create);
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
            auto res = core::fileStat(pb.path(), stat);
            CT_CHECK(!res.hasErr(), cErr);
            CT_CHECK(stat.size == 0, cErr);
            CT_CHECK(stat.type == core::FileType::Regular, cErr);
        }

        // Delete
        {
            auto res = core::fileDelete(pb.path());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

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
                auto res = core::fileRename(tc.path, "new name");
                CT_CHECK(res.hasErr());
            }
            {
                [[maybe_unused]] core::FileStat unused;
                auto res = core::fileStat(tc.path, unused);
                CT_CHECK(res.hasErr());
            }
            {
                [[maybe_unused]] core::ArrList<u8> unused;
                auto res = core::fileReadEntire(tc.path, unused);
                CT_CHECK(res.hasErr());
            }
            {
                [[maybe_unused]] core::ArrList<u8> unused(5, 0);
                auto res = core::fileWriteEntire(tc.path, unused);
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::dirCreate(tc.path);
                CT_CHECK(res.hasErr());
            }
            {
                auto res = core::dirRename(tc.path, "new name");
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
                auto res = core::dirDeleteRec(tc.path);
                CT_CHECK(res.hasErr());
            }
        }
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
    }

    return 0;
}

i32 edgeErrorCasesTest() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);
    pb.setFileName("test.txt");

    // Files opened for reading should fail to write
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.path(), core::OpenMode::Read | core::OpenMode::Create);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
        }
        {
            auto res = core::fileWrite(f, "should fail", core::cstrLen("should fail"));
            CT_CHECK(res.hasErr());
        }
        CT_CHECK(closeAndDeleteFile(std::move(f), pb.path()) == 0);
    }

    // Files oppned for writing should fail to read
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.path(), core::OpenMode::Write | core::OpenMode::Create);
            CT_CHECK(!res.hasErr());
            f = std::move(res.value());
        }
        {
            char buff[256] = {};
            auto res = core::fileRead(f, buff, 256);
            CT_CHECK(res.hasErr());
        }
        CT_CHECK(closeAndDeleteFile(std::move(f), pb.path()) == 0);
    }

    // Double closing a file
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.path(), core::OpenMode::Create);
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
            auto res = core::fileDelete(pb.path());
            CT_CHECK(!res.hasErr());
        }
        {
            auto res = core::fileDelete(pb.path()); // No crashes on double delete.
            CT_CHECK(res.hasErr());
        }
    }

    // Using a nullptr buffer with a valid descriptor.
    {
        core::FileDesc f;
        {
            auto res = core::fileOpen(pb.path(), core::OpenMode::Read | core::OpenMode::Write | core::OpenMode::Create);
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

        CT_CHECK(closeAndDeleteFile(std::move(f), pb.path()) == 0);
    }

    return 0;
}

i32 directoriesCreateRenameAndDeleteTest() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testNamesTable, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFileName(c);

        // Create directory
        {
            auto res = core::dirCreate(pb.path());
            CT_CHECK(!res.hasErr(), cErr);
        }

        TestPathBuilder renamedpb;
        renamedpb.setDirPath(testDirectory);
        renamedpb.setFileName("renamed");

        // Rename It
        {
            auto res = core::dirRename(pb.path(), renamedpb.path());
            CT_CHECK(!res.hasErr(), cErr);
        }

        // Stat It
        {
            core::FileStat stat;
            auto res = core::fileStat(renamedpb.path(), stat);
            CT_CHECK(!res.hasErr(), cErr);
            CT_CHECK(stat.type == core::FileType::Directory, cErr);
        }

        // Delete It
        {
            auto res = core::dirDelete(renamedpb.path());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 mostBasicReadAndWriteTest() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);
    pb.setFileName("test.txt");

    core::FileDesc f;
    {
        auto res = core::fileOpen(pb.path(), core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create);
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

    CT_CHECK(closeAndDeleteFile(std::move(f), pb.path()) == 0);

    return 0;
}

i32 basicListDirectoryContentsTest() {
    TestPathBuilder pb;

    pb.setDirPath(testDirectory);
    pb.appendToDirPath("test_directory");

    const char* basicFileNames[] = {
        "test0.txt",
        "test1.txt",
        "test2.txt",
    };
    constexpr addr_size basicFileNamesLen = sizeof(basicFileNames) / sizeof(basicFileNames[0]);

    const char* basicDirNames[] = {
        "test_sub_directory_0",
        "test_sub_directory_1",
    };
    constexpr addr_size basicDirNamesLen = sizeof(basicDirNames) / sizeof(basicDirNames[0]);

    // Create directory
    {
        auto res = core::dirCreate(pb.path());
        CT_CHECK(!res.hasErr());
    }

    // Create the test files inside the directory
    {
        for (addr_size i = 0; i < basicFileNamesLen; ++i) {
            pb.resetFilePart();
            pb.setFileName(basicFileNames[i]);

            core::FileDesc f;
            {
                auto res = core::fileOpen(pb.path(), core::OpenMode::Create);
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
            pb.setFileName(basicDirNames[i]);

            auto res = core::dirCreate(pb.path());
            CT_CHECK(!res.hasErr());
        }

        pb.resetFilePart(); // reset to the test direcotry

        // Make sure the root test directory is no longer considered empty.
        {
            auto res = core::dirIsEmpty(pb.path());
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
        auto res = core::dirWalk(pb.path(), listWalk,  reinterpret_cast<void*>(&closure));

        CT_CHECK(!res.hasErr());
        CT_CHECK(fileCount == basicFileNamesLen);
        CT_CHECK(dirCount == basicDirNamesLen);
    }

    // Delete directory
    {
        auto res = core::dirDeleteRec(pb.path());
        CT_CHECK(!res.hasErr());
    }

    return 0;
}

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

    TestPathBuilder pb;
    pb.setDirPath(testDirectory);

    i32 ret = core::testing::executeTestTable("test case failed at index: ", testCases, [&](auto& c, const char* cErr) {
        pb.resetFilePart();
        pb.setFileName(c.path);

        core::ArrList<u8> content(core::cstrLen(c.content));
        for (addr_size i = 0; i < core::cstrLen(c.content); ++i) {
            content.push(u8(c.content[i]));
        }

        // Write the file
        {
            auto res = core::fileWriteEntire(pb.path(), content);
            CT_CHECK(!res.hasErr(), cErr);
        }

        // Read the file
        {
            core::ArrList<u8> readBuffer;
            i32 counter = 5;
            while (counter-- > 0) {
                // Read the entire file a couple of times with the same buffer.
                auto res = core::fileReadEntire(pb.path(), readBuffer);
                CT_CHECK(!res.hasErr(), cErr);
                CT_CHECK(content.len() == readBuffer.len(), cErr);
                for (addr_size i = 0; i < content.len(); ++i) {
                    CT_CHECK(content[i] == readBuffer[i], cErr);
                }
            }
        }

        // Delete the file
        {
            auto res = core::fileDelete(pb.path());
            CT_CHECK(!res.hasErr(), cErr);
        }

        return 0;
    });
    CT_CHECK(ret == 0);

    return 0;
}

i32 seekWriteAndReadTest() {
    TestPathBuilder pb;
    pb.setDirPath(testDirectory);
    pb.setFileName("test.txt");

    core::FileDesc f;

    {
        auto res = core::fileOpen(pb.path(), core::OpenMode::Write | core::OpenMode::Read | core::OpenMode::Create);
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

    CT_CHECK(closeAndDeleteFile(std::move(f), pb.path()) == 0);

    return 0;
}

i32 runPltFileSystemTestsSuite() {
    using namespace core::testing;

    if (!createTestDirecotry()) {
        return -1;
    }

    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(createAndDeleteFileTest);
    if (runTest(tInfo, createAndDeleteFileTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(createFilesAndCheckIfTheyExistTest);
    if (runTest(tInfo, createFilesAndCheckIfTheyExistTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(checkFileStatsTest);
    if (runTest(tInfo, checkFileStatsTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(commonErrorsTest);
    if (runTest(tInfo, commonErrorsTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(edgeErrorCasesTest);
    if (runTest(tInfo, edgeErrorCasesTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(directoriesCreateRenameAndDeleteTest);
    if (runTest(tInfo, directoriesCreateRenameAndDeleteTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(mostBasicReadAndWriteTest);
    if (runTest(tInfo, mostBasicReadAndWriteTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(basicListDirectoryContentsTest);
    if (runTest(tInfo, basicListDirectoryContentsTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(readAndWriteEntireFileTest);
    if (runTest(tInfo, readAndWriteEntireFileTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    tInfo.name = FN_NAME_TO_CPTR(seekWriteAndReadTest);
    if (runTest(tInfo, seekWriteAndReadTest) != 0) { return -1; }
    if (!checkTestDirecotryIsCleanned()) { return -1; }

    return 0;
}
