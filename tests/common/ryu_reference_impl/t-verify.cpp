#include "../../t-index.h"

#include "./ryu.h"
#include "./ryu_parse.h"

constexpr bool eqFloats(auto a, auto b, bool eqPossible) {
    if (eqPossible) {
        return a == b;
    }

    // Compare bits:
    if constexpr (sizeof(decltype(a)) == 4) {
        u32 bitsa = core::bitCast<u32>(a);
        u32 bitsb = core::bitCast<u32>(b);
        return bitsa == bitsb;
    }
    else {
        u64 bitsa = core::bitCast<u64>(a);
        u64 bitsb = core::bitCast<u64>(b);
        return bitsa == bitsb;
    }
}

void verify(u32 x, u32 to) {
    u32 startX = x;

    while (x++ < to) {
        constexpr u32 BUFF_LEN = 64;
        char obuf[BUFF_LEN] = {};
        char gbuf[BUFF_LEN] = {};
        f32 f = core::bitCast<f32>(x);
        u32 n, gn;

        // TO STRING
        {
            n = u32(ryu::f2s_buffered_n(f, obuf));
            obuf[n] = '\0';

            // Handle difference for cases that end with zero exponent = E0
            if (n > 2) {
                if (obuf[n - 2] == 'E' && obuf[n - 1] == '0') {
                    n -= 2;
                }
            }

            gn = core::Unpack(core::floatToCstr(f, gbuf, BUFF_LEN));
            gbuf[gn] = '\0';

            if (!core::isinf(f)) {
                i32 cmpResult = core::memcmp(obuf, n, gbuf, gn);
                if (!(cmpResult == 0)) {
                    std::cout << x << std::endl;
                    Assert(false);
                }
            }
            else {
                if (f > 0) {
                    if (!(core::memcmp(obuf, n, "Infinity", core::cstrLen("Infinity")) == 0)) {
                        std::cout << x << std::endl;
                        Assert(false);
                    }
                    if (!(core::memcmp(gbuf, gn, "inf", core::cstrLen("inf")) == 0)) {
                        std::cout << x << std::endl;
                        Assert(false);
                    }
                }
                else {
                    if (!(core::memcmp(obuf, n, "-Infinity", core::cstrLen("-Infinity")) == 0)) {
                        std::cout << x << std::endl;
                        Assert(false);
                    }
                    if (!(core::memcmp(gbuf, gn, "-inf", core::cstrLen("-inf")) == 0)) {
                        std::cout << x << std::endl;
                        Assert(false);
                    }
                }
            }
        }

        // TO FLOAT
        {
            auto res = core::cstrToFloat<f32>(gbuf, gn);

            if (core::isnan(f) || core::isinf(f)) {
                // The reference implementation fails to parse special strings, which is a mistake in the algorithm.
                continue;
            }

            f32 e = 0;
            auto status = ryu::s2f_n(obuf, i32(n), &e);

            // Verify status
            if (res.hasValue()) {
                if (!(status == ryu::Status::SUCCESS)) {
                    std::cout << x << std::endl;
                    Assert(false);
                }
                f32 v = res.value();
                bool canCompare = !core::isnan(v) && !core::isnan(e);
                if (!(eqFloats(v, e, canCompare))) {
                    std::cout << x << std::endl;
                    Assert(false);
                }
            }
            else {
                if (!(status != ryu::Status::SUCCESS)) {
                    std::cout << x << std::endl;
                    Assert(false);
                }
            }
        }

        if (x % 100000 == 0) {
            std::cout << "Working... finished = " << x - startX << " left = " << to - x << std::endl;
        }
    }
}

i32 verifyRyuAlgorithm() {
    i32 coresN = core::Unpack(core::threadingGetNumCores());

    auto threads = core::ArrList<core::Thread>(addr_size(coresN));

    u32 x = 0;
    constexpr u32 xMax = core::limitMax<u32>();
    u32 dx = xMax / u32(coresN);

    struct TArgs {
        u32 x;
        u32 dx;
    };

    for (i32 i = 0; i < i32(coresN); i++) {
        core::Thread& t = threads[addr_size(i)];
        core::Expect(core::threadInit(t));

        auto& actx = core::getAllocator(core::DEFAULT_ALLOCATOR_ID);
        TArgs* tArgs = reinterpret_cast<TArgs*>(actx.alloc(1, sizeof(TArgs)));
        x += dx;
        tArgs->x = x;
        tArgs->dx = dx;

        Expect(core::threadStart(t, reinterpret_cast<void*>(tArgs), [](void* targ) {
            TArgs* argsp = reinterpret_cast<TArgs*>(targ);
            Assert(argsp);
            TArgs args = *argsp;
            verify(args.x, args.x + args.dx);
        }));
    }

    verify(x, xMax);

    for (i32 i = 0; i < i32(coresN); i++) {
        core::Thread& t = threads[addr_size(i)];
        Expect(core::threadJoin(t));
    }

    return 0;
}
