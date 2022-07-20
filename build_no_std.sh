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

g++ -DDEBUG=1 -DUSE_ASSERT=1 \
-nostdlib -O0 -ggdb -g -std=c++17 -fno-exceptions \
-Wall -Wno-unknown-pragmas -Wno-unused \
bootstrap.S -o build/test_no_std test_no_std.cpp ${SRC}

objdump -M intel -S build/test_no_std > build/test_no_std.S
echo "NEEDED section from readelf (!THE FOLLOWING LINES SHOULD BE EMPTY!) :"
readelf -d build/test_no_std | grep NEEDED

