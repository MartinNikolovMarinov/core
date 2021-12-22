mkdir -p build

pushd ../..

BUILD_DIR='./test/test_nostd_build'
SRC='
    src/char_ptr.cpp
    src/intrin.cpp
    src/ints.cpp
    src/math.cpp
    src/rnd.cpp
    src/mem.cpp
    src/bits.cpp
    src/utf.cpp
'

g++ -DDEBUG=1 -DUSE_ASSERT=1 \
-nostdlib -O0 -ggdb -g -std=c++17 -fno-exceptions \
-Wall -Wno-unknown-pragmas -Wno-unused \
${BUILD_DIR}/bootstrap.S -o ${BUILD_DIR}/build/main ${BUILD_DIR}/nostd_example.cpp ${SRC}

popd

objdump -M intel -S build/main > build/main.S
echo "IMPORTAT Just because it builds does not mean it can funciton without stdlib. The code needs to be tested..."
echo "NEEDED section from readelf (!THE FOLLOWING LINES SHOULD BE EMPTY!) :"
readelf -d build/main | grep NEEDED

