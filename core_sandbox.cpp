#include "tests/t-index.h"

#include <iostream>

void assertHandler(const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg) {
    constexpr u32 stackFramesToSkip = 2;
    constexpr addr_size stackTraceBufferSize = core::CORE_KILOBYTE * 8;
    char trace[stackTraceBufferSize] = {};
    addr_size traceLen = 0;
    bool ok = core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);

    std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                << "[ASSERTION]:\n  [EXPR]: " << failedExpr
                << "\n  [FUNC]: " << funcName
                << "\n  [FILE]: " << file << ":" << line
                << "\n  [MSG]: " << (errMsg ? errMsg : "") // IMPORTANT: MSVC's std implementation will crash if errMsg is nullptr !
                << ANSI_RESET()
                << std::endl;
    std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;

    if (!ok) {
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "Failed to take full stacktrace. Consider resizing the stacktrace buffer size!"
                  << ANSI_RESET()
                  << std::endl;
    }

    throw std::runtime_error("Assertion failed!");
}

i32 exampleWorkload(i32 x) {
    i32 a = x * 17 + 3;
    i32 b = a ^ (a >> 2);
    i32 c = (u32(b) * 2654435761u) & 0x7fffffff;
    i32 d = (c >> 3) ^ (c << 5);
    i32 e = (d + a - b) * (x | 1);

    for (i32 i = 0; i < 16; ++i) {
        e = (e ^ (e << 13)) + (i * 37);
        e = (e ^ (e >> 7)) * 31;
        e ^= (e << 11);
    }

    if (e & 1) {
        e = (e * 3) ^ (e >> 1);
    } else {
        e = (e + 7) ^ (e << 2);
    }

    return e ^ (e >> 16);
}

core::Profiler profiler_1;

enum ProfilePoints {
    PP_RESERVED,

    PP_G,

    PP_Example1,
    PP_Example1_2,
    PP_Example1_3,
    PP_Example1_4,
    PP_Example2,
};

// i32 main() {
//     core::initProgramCtx(assertHandler, nullptr);

//     i32 ret = 0;
//     constexpr addr_size N_REPEAT = 10000000;

//     profiler_1.beginProfile();

//     {
//         TIME_BLOCK(profiler_1, PP_G, "G");

//         {
//             TIME_BLOCK(profiler_1, PP_Example1, "Example 1");
//             for (addr_size i = 0; i < N_REPEAT; i++) {
//                 TIME_BLOCK(profiler_1, PP_Example1_4, "Example 1.4");
//                 ret += exampleWorkload(i32(i));
//             }

//             {
//                 TIME_BLOCK(profiler_1, PP_Example1_2, "Example 1.2");
//                 for (addr_size i = N_REPEAT / 2 - 1; i < N_REPEAT; i++) {
//                     TIME_BLOCK(profiler_1, PP_Example1_3, "Example 1.3");
//                     ret += exampleWorkload(i32(i));
//                 }
//             }
//         }

//         {
//             TIME_BLOCK(profiler_1, PP_Example2, "Example 2");
//             for (addr_size i = 0; i < N_REPEAT; i++) {
//                 ret += exampleWorkload(i32(i));
//             }
//         }

//     }

//     auto pRes = profiler_1.endProfile();
//     logInfo("Profiler 1");
//     pRes.logResult(core::LogLevel::L_INFO);

//     return ret;
// }

template <i32 TBufferSize>
struct StaticPathBuilder {
    char buff[TBufferSize] = {};
    i32 len = 0;

    //==================================================================================================================
    // Get Parts
    //==================================================================================================================

    constexpr const char* fullPath() const { 
        return buff;
    }
    constexpr core::StrView fullPathSv() const { 
        auto ret = core::sv(buff, addr_size(len));
        return ret;
    }

    constexpr const char* filePart() const {
        return filePartSv().data();
    }
    constexpr core::StrView filePartSv() const {
        addr_off fOff = fileOff();
        if (buff[fOff] == core::PATH_SEPARATOR) fOff++;
        auto ret = core::sv(buff + fOff, addr_size(addr_off(len) - fOff));
        return ret;
    }

    constexpr const char* extPart() const { 
        return extPartSv().data();
    }
    constexpr inline core::StrView extPartSv() const {
        const char* fp = filePart();
        addr_size fpLen = addr_size(buff + len - fp);
        addr_off dotRel = core::findLast(fp, fpLen, [](char c, addr_size) { return c == '.'; });
        if (dotRel < 0) return {}; // no ext part
        addr_size startRel = addr_size(dotRel + 1);
        auto ret = core::sv(fp + startRel, fpLen - startRel);
        return ret;
    }

    constexpr const char* dirPart() const { 
        return dirPartSv().data();
    }
    constexpr inline core::StrView dirPartSv() const {
        addr_off lastSepIdx = core::findLast(buff, addr_size(len), [](char c, addr_size) { return c == core::PATH_SEPARATOR; });
        if (lastSepIdx < 0) return {};
        auto ret = core::sv(buff, addr_size(lastSepIdx + 1));
        return ret;
    }

    //==================================================================================================================
    // Set Parts
    //==================================================================================================================

    constexpr inline StaticPathBuilder& setDirPart(core::StrView directory) {
        if (directory.empty()) return *this;

        assertWriteSize(0, directory.len());
        
        len = i32(core::memcopy(buff, directory.data(), directory.len()));

        if (buff[len - 1] != core::PATH_SEPARATOR) {
            buff[len++] = core::PATH_SEPARATOR;
        }
        buff[len] = '\0';
        
        debugClearBufferAfterLen();
        return *this;
    }

    constexpr inline StaticPathBuilder& setFilePart(core::StrView file) {
        if (file.empty()) {
            return *this;
        }

        addr_off fOff = fileOff();

        assertWriteSize(addr_size(fOff), file.len());

        if (buff[fOff] == core::PATH_SEPARATOR) {
            fOff++; // Skip the path separator
        }

        core::memcopy(buff + fOff, file.data(), file.len());

        len = i32(fOff) + i32(file.len());
        buff[len] = '\0';

        debugClearBufferAfterLen();
        return *this;
    }

    constexpr inline StaticPathBuilder& setExtPart(core::StrView ext) {
        if (ext.empty()) return *this;

        addr_off eOff = extOff();

        assertWriteSize(addr_size(eOff), ext.len());

        // Add an extra dot if input str does not start with it and the current buffer does not end with it:
        if (ext.first() != '.') {
            if (buff[eOff] != '.') {
                buff[eOff] = '.';
            }
            eOff++; // skip dot
        }

        core::memcopy(buff + eOff, ext.data(), ext.len());

        len = i32(eOff) + i32(ext.len());
        buff[len] = '\0';

        debugClearBufferAfterLen();
        return *this;
    }

    //==================================================================================================================
    // Rest
    //==================================================================================================================

    constexpr inline void resetFilePart() {
        addr_off fOff = fileOff();
        if (fOff >= 0) {
            if (buff[fOff] == core::PATH_SEPARATOR) {
                fOff++;
                // This should not be possible, but sanity check it anyway:
                Panic(fOff < TBufferSize, "[BUG] StaticPathBuilder: buffer overflow.");
            }
            
            len = i32(fOff);
            buff[len] = '\0';
        }
        
        debugClearBufferAfterLen();
    }

    constexpr inline void reset() {
        len = 0;
        buff[0] = '\0';

        debugClearBufferAfterLen();
    }

private:
    constexpr inline void assertWriteSize(addr_size start, addr_size writeSize) {
        addr_size writeEndOff = start + writeSize;
        // Enforcing +2 because of the null terminator and a possible extra '\' or '.' symbols:
        Panic(writeEndOff + 2 < addr_size(TBufferSize), "StaticPathBuilder: buffer overflow");
    } 

    constexpr inline addr_off fileOff() const { return const_cast<StaticPathBuilder*>(this)->fileOff(); }
    constexpr inline addr_off fileOff() {
        addr_off fOff = core::findLast(buff, addr_size(len), [](char x, addr_size) { return x == core::PATH_SEPARATOR; });
        if (fOff < 0) return 0;
        return fOff;
    }
 
    constexpr inline addr_off extOff() const { return const_cast<StaticPathBuilder*>(this)->extOff(); }
    constexpr inline addr_off extOff() {
        addr_off fOff = fileOff();
        if (fOff == 0) return 0;
        addr_size fileLen = addr_size(len - i32(fOff) - 1);
        addr_off eOff = core::findLast(buff + fOff, fileLen, [](char x, addr_size) { return x == '.'; });
        if (eOff < 0) return len;
        return eOff + fOff;
    }

    constexpr inline void debugClearBufferAfterLen() {
#if CORE_DEBUG
        core::memset(buff + len + 1, '-', addr_size(TBufferSize - len - 1));
#endif
    }
};

template <typename T>
void dump(const char* label, const T& pb)
{
    auto full = pb.fullPathSv();
    auto file = pb.filePartSv();
    auto dir = pb.dirPartSv();

    logInfo("[{}]", label);
    logInfo("  full: '{}' ({})", full, full.len());
    logInfo("  file: '{}' ({})", file, file.len());
    logInfo("  dir: '{}' ({})", dir, dir.len());

    const auto ext = pb.extPartSv();
    logInfo("  ext : '{}' ({})", ext, ext.len());

    Assert(dir.len() + file.len() == full.len());

    logInfo("");
}

i32 main()
{
    core::initProgramCtx(assertHandler, nullptr);
    defer { core::destroyProgramCtx(); };

    StaticPathBuilder<64> pb = {};

    pb.setDirPart("dir"_sv);
    dump("added dir", pb);
    pb.setFilePart("file"_sv);
    dump("added file", pb);
    pb.setExtPart("ext"_sv);
    dump("added ext", pb);
    pb.reset();
    dump("after rest", pb);

    return 0;
}

void testSetExtPartTest() {
    StaticPathBuilder<64> pb = {};

    pb.setDirPart("directory"_sv);
    pb.setFilePart("file"_sv);
    pb.setExtPart("ext"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.setExtPart("ext2"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.setFilePart("over"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.setExtPart("ext3"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.setDirPart("dirover"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.resetFilePart();
    pb.setExtPart("nofileext"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.reset();
    pb.setExtPart("onlyext"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.reset();
    pb.setExtPart(""_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.reset();
    pb.setFilePart("."_sv);
    pb.setExtPart("."_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.reset();
}

void testSetFilePartTest() {
    StaticPathBuilder<64> pb = {};

    pb.setDirPart("test"_sv);
    pb.setFilePart("example"_sv);
    
    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());
    
    pb.setFilePart("change"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());
    
    pb.reset();

    pb.setFilePart("no_directory_file"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.setFilePart("k"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.resetFilePart();

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.setDirPart("dir"_sv);
    pb.setFilePart("file_name"_sv);
    pb.resetFilePart();

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());

    pb.reset();
}

void testSetDirPartTest() {
    StaticPathBuilder<64> pb = {};

    pb.setDirPart("testing/this"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());
    pb.reset();

    pb.setDirPart("testing/this/"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());
    pb.reset();

    pb.setDirPart("test"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());
    pb.reset();

    pb.setDirPart(""_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());
    pb.reset();

    pb.setDirPart("/"_sv);

    logInfo("Full Path SV: {}", pb.fullPathSv());
    logInfo("Full Path: {}", pb.fullPath());
    pb.reset();
}