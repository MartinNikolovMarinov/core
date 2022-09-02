mkdir -p build

SRC='
    ./src/*.cpp
'

# IMPORTANT:
# When compiling with -nostdlib you need -ffreestanding, otherwise the compiler might use stdlib functions to perform optimizations.
# This can have the effect of significanltly reducing performance.
# Use nostdlib in cases where you know exactly what you are doing!

echo "Compiling the following files:"
ls $SRC
echo ""

g++ -DDEBUG=1 -DUSE_ASSERT=1 \
-nostdlib -O2 -std=c++17 -ffreestanding -fno-exceptions \
-Wall -Wno-unknown-pragmas -Wno-unused \
bootstrap.S -o build/test_no_std test_no_std.cpp ${SRC}

objdump -M intel -S build/test_no_std > build/test_no_std.S
echo "grepping for NEEDED section in generated elf (should be empty line):"
readelf -d build/test_no_std | grep NEEDED

