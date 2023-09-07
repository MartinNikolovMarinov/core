#pragma once

#include <API.h>
#include <types.h>
#include <expected.h>

namespace core {

using namespace coretypes;

enum struct CORE_API_EXPORT seek_origin {
    Begin = 0,
    Current = 1,
    End = 2
};

struct CORE_API_EXPORT io_err {
    bool eof = false;
    const char* errCptr = nullptr; // should be a literal expression cptr.
};

struct CORE_API_EXPORT ireader {
    virtual core::expected<addr_size, io_err> read(void* out, addr_size size) = 0;
};

struct CORE_API_EXPORT iwriter {
    virtual core::expected<addr_size, io_err> write(const void* in, addr_size size) = 0;
};

struct CORE_API_EXPORT icloser {
    virtual core::expected<io_err> close() = 0;
};

struct CORE_API_EXPORT iseeker {
    virtual core::expected<io_err> seek(addr_off offset, seek_origin origin) = 0;
};

struct CORE_API_EXPORT ireader_writer : public ireader, public iwriter {};
struct CORE_API_EXPORT ireader_closer : public ireader, public icloser {};
struct CORE_API_EXPORT iwriter_closer : public iwriter, public icloser {};
struct CORE_API_EXPORT ireader_writer_closer : public ireader, public iwriter, public icloser {};

// TODO: Implement write_full and read_full.

CORE_API_EXPORT core::expected<io_err> write_full(iwriter& writer, void* out, addr_size size);

} // namespace core
