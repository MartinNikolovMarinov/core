
template <addr_size ReadBlockSize>
struct test_str_buffer : public core::ireader_writer_closer, public core::iseeker {
    constexpr static addr_size BUF_MAX_SIZE = 1024;
    static_assert(ReadBlockSize <= BUF_MAX_SIZE, "ReadBlockSize must be less than BUF_MAX_SIZE");

    char buf[BUF_MAX_SIZE] = {};
    addr_off off = 0;
    bool closed = false;

    core::expected<core::addr_size, core::io_err> read(void* out, core::addr_size size) override {
        if (off + addr_off(size) > addr_off(BUF_MAX_SIZE)) {
            return core::unexpected(core::io_err { false, "read out of bounds" });
        }
        size = core::min(size, ReadBlockSize);
        core::memcopy(out, buf + off, size);
        off += size;
        return core::move(size);
    }

    core::expected<core::addr_size, core::io_err> write(const void* in, core::addr_size size) override {
        if (off + addr_off(size) > addr_off(BUF_MAX_SIZE)) {
            return core::unexpected(core::io_err { false, "write out of bounds" });
        }
        size = core::min(size, ReadBlockSize);
        core::memcopy(buf + off, in, size);
        off += size;
        return core::move(size);
    }

    core::expected<core::io_err> close() override {
        closed = true;
        return { };
    }

    core::expected<core::io_err> seek(core::addr_off offset, core::seek_origin origin) override {
        addr_off nextOff = off;
        switch (origin) {
            case core::seek_origin::Begin:   nextOff = offset;                          break;
            case core::seek_origin::Current: nextOff += offset;                         break;
            case core::seek_origin::End:     nextOff = addr_off(BUF_MAX_SIZE) - offset; break;
        }
        if (nextOff < 0 || addr_size(nextOff) > BUF_MAX_SIZE) {
            return core::unexpected(core::io_err { false, "seek out of bounds" });
        }
        off = nextOff;
        return { };
    }
};

i32 str_buffer_test() {
    test_str_buffer<5> buffer;
    defer { Expect(buffer.close()); };
    {
        auto res = buffer.write("abcdefghij", 10);
        Assert(!res.has_err());
        Assert(res.value() == 5);
        Assert(buffer.off == 5);
    }
    {
        auto res = core::write_full(buffer, "klmnopqrstu", 11);
        Assert(!res.has_err());
        Assert(buffer.off == 16);
    }
    {
        auto res = buffer.seek(0, core::seek_origin::Begin);
        Assert(!res.has_err());
        Assert(buffer.off == 0);
    }
    {
        char out[16] = {};
        auto res = core::read_full(buffer, out, 16);
        Assert(!res.has_err());
        Assert(core::cptr_eq(out, "abcdeklmnopqrstu", 16));
        Assert(buffer.off == 16);
    }
    {
        auto res = buffer.seek(0, core::seek_origin::Begin);
        Assert(!res.has_err());
        Assert(buffer.off == 0);
    }
    {
        char out[10] = {};
        auto res = core::read_full(buffer, out, 10);
        Assert(!res.has_err());
        Assert(core::cptr_eq(out, "abcdeklmno", 10));
        Assert(buffer.off == 10);
    }
    return 0;
}

i32 run_io_tests_suite() {
    RunTest(str_buffer_test);

    return 0;
}
