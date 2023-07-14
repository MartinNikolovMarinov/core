pushd build

# TODO: This script needs to run compiletime tests as well!

echo "Running tests with gcc DEBUG:"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc
make -j
if [ $? -ne 0 ]; then
    echo "DEBUG gcc build failed"
    exit 1
fi
./test
if [ $? -ne 0 ]; then
    echo "DEBUG gcc tests failed"
    exit 1
fi

echo "Running tests with clang DEBUG:"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang
make -j
if [ $? -ne 0 ]; then
    echo "DEBUG clang build failed"
    exit 1
fi
./test
if [ $? -ne 0 ]; then
    echo "DEBUG clang tests failed"
    exit 1
fi

echo "Running tests with gcc RELEASE:"
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc
make -j
if [ $? -ne 0 ]; then
    echo "RELEASE gcc build failed"
    exit 1
fi
./test
if [ $? -ne 0 ]; then
    echo "RELEASE gcc tests failed"
    exit 1
fi

echo "Running tests with clang RELEASE:"
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang
make -j
if [ $? -ne 0 ]; then
    echo "RELEASE clang build failed"
    exit 1
fi
./test
if [ $? -ne 0 ]; then
    echo "RELEASE clang tests failed"
    exit 1
fi

popd # go back to root

echo "Running tests with no stdlib:"
bash build_no_std.sh
if [ $? -ne 0 ]; then
    echo "No stdlib build failed"
    exit 1
fi
./build/test_no_std
if [ $? -ne 0 ]; then
    echo "No stdlib tests failed"
    exit 1
fi
