#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_expected.h>
#include <core_utils.h>

#include <plt/core_plt_error.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT const char* osErrorDescribe(PltErrCode err);

CORE_API_EXPORT expected<void*, PltErrCode> osAllocPages(size_t count);
CORE_API_EXPORT expected<PltErrCode> osFreePages(void* addr, size_t count);

CORE_API_EXPORT expected<u64, PltErrCode> osUnixTimestampInMs();

struct CORE_API_EXPORT FileDesc {
    void* handle;

    NO_COPY(FileDesc);

    FileDesc();
    FileDesc(void* handle);

    FileDesc(FileDesc&& other);
    FileDesc& operator=(FileDesc&& other);
};

enum struct CORE_API_EXPORT FileType {
    None,

    Regular,
    Directory,
    Symlink,
    Other,

    SENTINEL
};

constexpr const char* fileTypeToCptr(FileType type) {
    switch (type) {
        case FileType::None:      return "None";
        case FileType::Regular:   return "Regular";
        case FileType::Directory: return "Directory";
        case FileType::Symlink:   return "Symlink";
        case FileType::Other:     return "Other";
        case FileType::SENTINEL:  return "SENTINEL";
    }

    Assert(false, "Invalid FileType");
    return "None";
}

/**
 * The file system api should implement functions to:
 *
 * * Open a file
 * * Close a file
 * * Create a file
 * * Remove a file
 *
 * * Read from a file
 * * Write to a file
 * * Seek in a file
 * * Read and write at a specific offset without modifing the position (pwrite, pread)
 * * Rename a file
 * * Truncate a file
 * * Read entire file
 *
 * * Get the size of a file
 * * Get last modified time of a file
 * * Get creation time of a file
 *
 * * Open a directory
 * * Close a directory
 * * Create a directory
 * * Remove a directory
 *
 * * List the contents of a directory
 * * Rename a directory
 *
 * * Open the current working directory
 * * Change current working directory
 * * Get the current working directory
 *
 * * Flush a file descriptor
 **/

/**
 * Other things to implement:
 *
 * * Register signal handlers for
 *      CORE_SIGABRT,  // Abnormal termination
 *      CORE_SIGFPE,   // Floating-point error most likely caused by division by zero
 *      CORE_SIGILL,   // Illegal instruction
 *      CORE_SIGINT,   // CTRL+C signal
 *      CORE_SIGSEGV,  // Illegal storage access most likely caused by dereferencing a bad/null pointer
 *      CORE_SIGTERM,  // Termination request
 *      CORE_SIGKILL,  // Kill the process
 *      CORE_SIGSTOP,  // Stop the process
 *
 **/

} // namespace core

