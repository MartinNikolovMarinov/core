#include <core_mem.h>
#include <testing/testing_framework.h>

namespace core::testing {

char* elapsedTimeToStr(char out[256], std::chrono::nanoseconds deltaTime) {
    constexpr u32 kBufferSize = 256;

    core::memset(reinterpret_cast<u8*>(&out[0]), u8(0), kBufferSize);

    if (deltaTime >= std::chrono::hours(1)) {
        i32 hours = i32(std::chrono::duration_cast<std::chrono::hours>(deltaTime).count());
        i32 minutes = i32(std::chrono::duration_cast<std::chrono::minutes>(deltaTime).count() % 60);
        i32 seconds = i32(std::chrono::duration_cast<std::chrono::seconds>(deltaTime).count() % 60);
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count() % 1000;
        snprintf(&out[0], kBufferSize, "%02dh %02dm %02ds %lldms", hours, minutes, seconds, static_cast<long long int>(milliseconds));
    }
    else if (deltaTime >= std::chrono::minutes(1)) {
        i32 minutes = i32(std::chrono::duration_cast<std::chrono::minutes>(deltaTime).count());
        i32 seconds = i32(std::chrono::duration_cast<std::chrono::seconds>(deltaTime).count() % 60);
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count() % 1000;
        snprintf(&out[0], kBufferSize, "%02dm %02ds %lldms", minutes, seconds, static_cast<long long int>(milliseconds));
    }
    else if (deltaTime >= std::chrono::seconds(1)) {
        i32 seconds = i32(std::chrono::duration_cast<std::chrono::seconds>(deltaTime).count());
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count() % 1000;
        snprintf(&out[0], kBufferSize, "%02ds %lldms", seconds, static_cast<long long int>(milliseconds));
    }
    else if (deltaTime >= std::chrono::milliseconds(1)) {
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count();
        snprintf(&out[0], kBufferSize, "%lldms", static_cast<long long int>(milliseconds));
    }
    else {
        i64 nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime).count();
        snprintf(&out[0], kBufferSize, "%lldns", static_cast<long long int>(nanoseconds));
    }

    return out;
};

char* memoryUsedToStr(char out[128], addr_size deltaMemory) {
    constexpr u32 kBufferSize = 128;

    core::memset(reinterpret_cast<u8*>(&out[0]), u8(0), kBufferSize);

    if (deltaMemory >= 1024 * 1024 * 1024) {
        f64 gb = f64(deltaMemory) / (1024.0 * 1024.0 * 1024.0);
        snprintf(&out[0], kBufferSize, "%.2f GB", gb);
    }
    else if (deltaMemory >= 1024 * 1024) {
        f64 mb = f64(deltaMemory) / (1024.0 * 1024.0);
        snprintf(&out[0], kBufferSize, "%.2f MB", mb);
    }
    else if (deltaMemory >= 1024) {
        f64 kb = f64(deltaMemory) / 1024.0;
        snprintf(&out[0], kBufferSize, "%.2f KB", kb);
    }
    else {
        snprintf(&out[0], kBufferSize, "%lld B", static_cast<long long int>(deltaMemory));
    }

    return out;
}

} // namespace core::testing
