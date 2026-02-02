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

    Assert(dir.len() + file.len() == full.len(), "DirLen + FileLen should be equal to FullLen");

    logInfo("");
}

void testBasicSetup() {
    StaticPathBuilder<64> pb = {};

    pb.setDirPart("dir"_sv);
    dump("added dir", pb);
    pb.appendToDirPath("nested"_sv);
    dump("append dir", pb);
    pb.setFilePart("file"_sv);
    dump("added file", pb);
    pb.setExtPart("ext"_sv);
    dump("added ext", pb);
    pb.reset();
    dump("after rest", pb);
}

void testSetExtPartTest() {
    StaticPathBuilder<64> pb = {};

    pb.setDirPart("directory"_sv);
    pb.setFilePart("file"_sv);
    pb.setExtPart("ext"_sv);

    dump("All 3 set", pb);

    pb.setExtPart("ext2"_sv);

    dump("Change ext part", pb);

    pb.setFilePart("over"_sv);

    dump("Override file part", pb);

    pb.setExtPart("ext3"_sv);

    dump("Change ext part", pb);

    pb.setDirPart("dirover"_sv);

    dump("Override directory part", pb);

    pb.resetFilePart();
    pb.setExtPart("nofileext"_sv);

    dump("Set ext without file part", pb);

    pb.reset();
    pb.setExtPart("onlyext"_sv);

    dump("Set ext part only", pb);

    pb.reset();
    pb.setExtPart(""_sv);

    dump("Nothing", pb);

    pb.reset();
    pb.setFilePart("."_sv);
    pb.setExtPart("."_sv);

    dump("Ext part is a dot", pb);

    pb.reset();
}

void testSetFilePartTest() {
    StaticPathBuilder<64> pb = {};

    pb.setDirPart("test"_sv);
    pb.setFilePart("example"_sv);
    
    dump("Dir part and file part", pb);
    
    pb.setFilePart("change"_sv);

    dump("change file part", pb);
    
    pb.reset();

    pb.setFilePart("no_directory_file"_sv);

    dump("reset and set only file part", pb);

    pb.setFilePart("k"_sv);

    dump("set to short file part", pb);

    pb.resetFilePart();

    dump("reset file part", pb);

    pb.setDirPart("dir"_sv);
    pb.setFilePart("file_name"_sv);
    pb.resetFilePart();

    dump("set both and reset file part", pb);

    pb.reset();
}

void testSetDirPartTest() {
    StaticPathBuilder<64> pb = {};

    pb.setDirPart("testing/this"_sv);

    dump("dir part with 2 components NO slash at the end", pb);
    pb.reset();

    pb.setDirPart("testing/this/"_sv);

    dump("dir part with 2 components with slash at the end", pb);
    pb.reset();

    pb.setDirPart("test"_sv);

    dump("dir part 1 component NO slash at the end", pb);
    pb.reset();

    pb.setDirPart(""_sv);

    dump("empty dir part", pb);
    pb.reset();

    pb.setDirPart("/"_sv);

    dump("root dir part", pb);
    pb.reset();
}

i32 main() {
    core::initProgramCtx(assertHandler, nullptr);
    defer { core::destroyProgramCtx(); };

    // testBasicSetup();
    // testSetExtPartTest();
    // testSetFilePartTest();
    // testSetDirPartTest();

    return 0;
}