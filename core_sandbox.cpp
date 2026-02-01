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

void dump(const char* label, const core::StaticPathBuilder<64>& pb)
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

    core::StaticPathBuilder<64> pb = {};

    dump("default", pb);

    pb.setFilePart("t/tt.txt");
    dump("file with ext", pb);

    pb.setExtPart("bin");
    dump("changed ext via setExtPart", pb);

    pb.setFilePart("tt/tt/asd.cs");
    dump("nested + ext", pb);

    pb.setExtPart("h");
    dump("short ext", pb);

    pb.setFilePart("t/"); // no file and no ext
    dump("trailing slash", pb);

    pb.setFilePart("noext"); // no ext
    dump("no extension", pb);

    pb.setFilePart("a."); // ext exists but empty
    dump("empty extension", pb);

    pb.setFilePart("/123456"); // leading slash, no ext
    dump("leading slash", pb);

    pb.setFilePart("dir.with.dot/file"); // dot in directory MUST NOT count as ext
    dump("dot in directory (should be unset)", pb);

    pb.setFilePart("dir.with.dot/file.txt");
    dump("dot in directory + file ext", pb);

    pb.setFilePart("/");
    dump("empty file part", pb);

    pb.setFilePart({});
    dump("null file part", pb);

    pb.setFilePart("yoyoy.asd");
    pb.resetFilePart();
    dump("reset file part", pb);

    pb.appendToDirPath("testis/");
    dump("appended testis", pb);

    pb.appendToDirPath("banica/");
    dump("appended banica", pb);

    pb.setFilePart("yo");
    dump("yo", pb);

    pb.setDirPart("resetted baby");
    dump("resetted baby", pb);

    pb.setFilePart("bace");
    dump("bace", pb);

    pb.setExtPart("jpg");
    dump("jpg", pb);

    pb.setExtPart("png");
    dump("png", pb);

    return 0;
}
