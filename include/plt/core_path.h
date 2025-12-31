#pragma once

#include <core_API.h>
#include <core_assert.h>
#include <core_cstr.h>
#include <core_mem.h>
#include <core_str_view.h>
#include <core_traits.h>
#include <core_types.h>

namespace core {

static constexpr char PATH_SEPARATOR = '/';

template <typename T>
concept Path =
    requires {
        requires std::is_same_v<decltype(&T::fullPath), const char* (T::*)() const>;
        requires std::is_same_v<decltype(&T::filePart), const char* (T::*)() const>;
        requires std::is_same_v<decltype(&T::extPart), const char* (T::*)() const>;
    };

struct CORE_API_EXPORT ImmutablePath {
    const char* ptr;
    const addr_size len;

    constexpr ImmutablePath(const char* p, addr_size l) : ptr(p), len(l) {}
    constexpr ImmutablePath(const char* p) : ptr(p), len(core::cstrLen(p)) {}

    constexpr const char* filePart() const {
        if (len == 0) {
            return ptr;
        }

        const char* p = ptr + len;
        while (p != ptr) {
            if (*(p - 1) == PATH_SEPARATOR) {
                return p;
            }
            --p;
        }

        return ptr;
    }

    constexpr const char* extPart() const {
        const char* start = filePart();
        const char* p = ptr + len;
        while (p != start) {
            if (*(p - 1) == '.') {
                return p - 1;
            }
            --p;
        }

        return ptr + len;
    }

    constexpr const char* fullPath() const { return ptr; }
};

static_assert(Path<ImmutablePath>, "ImmutablePath does not satisfy Path concept");

// TODO: Write tests for this.
template<i32 TBufferSize>
struct StaticPathBuilder {
    static constexpr char FILE_SEPARATOR = '/';

    char buff[TBufferSize];
    addr_size dirPathLen;

    StaticPathBuilder() : buff{}, dirPathLen(0) {}

    void setDirPath(const char* dirPath) { return setDirPath(core::sv(dirPath)); }
    void setDirPath(core::StrView dirPath) {
        Assert(TBufferSize > dirPath.len(), "overflow");
        dirPathLen = core::memcopy(buff, dirPath.data(), dirPath.len());
        buff[dirPathLen] = '\0';
    }

    void appendToDirPath(const char* dirName) { return appendToDirPath(core::sv(dirName)); }
    void appendToDirPath(core::StrView dirName) {
        Assert(TBufferSize > dirPathLen + addr_size(FILE_SEPARATOR) + dirName.len(), "overflow");
        buff[dirPathLen++] = FILE_SEPARATOR;
        dirPathLen += core::memcopy(buff + dirPathLen, dirName.data(), dirName.len());
        buff[dirPathLen] = '\0';
    }

    constexpr const char* fullPath() const { return buff; }

    const char* fileName() const { return buff + dirPathLen + 1; }

    char* filePartMutable() { return buff + dirPathLen; }
    const char* filePart() const { return buff + dirPathLen; }

    const char* extPart() const {
        const char* start = fileName();
        const char* end = buff + core::cstrLen(buff);
        const char* p = end;
        while (p != start) {
            if (*(p - 1) == '.') {
                return p - 1;
            }
            --p;
        }
        return end;
    }

    addr_size filePartLen() { return core::cstrLen(filePartMutable()); }

    void setFilePart(const char* fileName) { return setFilePart(core::sv(fileName)); }
    void setFilePart(core::StrView fileName) {
        Assert(TBufferSize > dirPathLen +  addr_size(FILE_SEPARATOR) + fileName.len(), "overflow");
        filePartMutable()[0] = FILE_SEPARATOR;
        core::memcopy(filePartMutable() + 1, fileName.data(), fileName.len());
        filePartMutable()[fileName.len() + 1] = '\0';
    }

    void resetFilePart() {
        core::memset(filePartMutable(), char(0), filePartLen());
    }

    void reset() {
        core::memset(buff, char(0), sizeof(buff));
        dirPathLen = 0;
    }
};

static_assert(Path<StaticPathBuilder<255>>, "StaticPathBuilder does not satisfy Path concept");

} // namespace core
