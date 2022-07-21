mkdir -p build

SRC='
    ./src/char_ptr.cpp
    ./src/intrin.cpp
    ./src/ints.cpp
    ./src/math.cpp
    ./src/rnd.cpp
    ./src/mem.cpp
    ./src/bits.cpp
    ./src/utf.cpp
    ./src/utils.cpp
'

# IMPORTANT:
# When compiling with -nostdlib you need -ffreestanding, otherwise the compiler might use stdlib functions to perform optimizations.
# This can have the effect of significanltly reducing performance.
# Use nostdlib in cases where you know exactly what you are doing!

g++ -DDEBUG=1 -DUSE_ASSERT=1 \
-nostdlib -O2 -std=c++17 -ffreestanding -fno-exceptions \
-Wall -Wno-unknown-pragmas -Wno-unused \
bootstrap.S -o build/test_no_std test_no_std.cpp ${SRC}

objdump -M intel -S build/test_no_std > build/test_no_std.S
echo "NEEDED section from readelf (!THE FOLLOWING LINES SHOULD BE EMPTY!) :"
readelf -d build/test_no_std | grep NEEDED

