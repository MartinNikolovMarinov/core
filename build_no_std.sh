mkdir -p build

SRC="./src/*.cpp"
INCLUDES="-I $(pwd)/src"

# IMPORTANT:
# When compiling with -nostdlib you need -ffreestanding, otherwise the compiler might use stdlib functions to perform
# optimizations. This flag can significanltly reducing performance. Use nostdlib in cases where you know exactly what
# you are doing!

echo -e "Compiling the following files: $(ls $SRC)\n"

# NOTE: Add the -g options and set the optiomization level to -O0 to debug the code!
g++ -DCORE_DEBUG=1 -DCORE_ASSERT_ENABLED=1 -DRUN_COMPILETIME_TESTS=1 \
-nostdlib -O2 -std=c++17 -ffreestanding -fno-exceptions \
-Wall -Wextra -Wno-unknown-pragmas -Wno-unused-function \
$INCLUDES \
bootstrap.S -o build/test_no_std test_no_std.cpp ${SRC}

# Check if the NEEDED section is empty:
objdump -M intel -S build/test_no_std > build/test_no_std.S
echo "grepping for NEEDED section in generated elf (should be empty line):"
readelf -d build/test_no_std | grep NEEDED

exit 0
