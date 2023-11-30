```c++

#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_expected.h>
#include <core_utils.h>

#include <plt/core_plt_error.h>

namespace core {

using namespace coretypes;

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
 * * Open a file (done)
 * * Close a file (done)
 * * Create a file (done)
 * * Remove a file (done)
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
 * * List the contents of a directory
 * * Close a directory
 * * Create a directory (done)
 * * Remove a directory (done)
 * * Rename a directory (done)
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
 *   Do these work per thread ? I am not sure exactly. Need a bit more research before starting the implementation.
 *
 **/

} // namespace core

```
