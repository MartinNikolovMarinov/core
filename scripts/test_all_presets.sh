#!/bin/bash

clean_build_dir() {
    mkdir -p build
    rm -rf build/*
}

check_exit_code() {
    if [ $? -ne 0 ]; then
        echo "Build failed"
        exit 1
    fi
}

function prompt_user_to_continue() {
    while true; do
        read -p "$* [y/n]: " yn
        case $yn in
            [Yy]*) return 0 ;;
            [Nn]*) return 1 ;;
        esac
    done
}

build_preset() {
    echo "BUILD WITH PRESET = $1"
    cmake .. --preset "$1"
    check_exit_code
    make -j
    check_exit_code
}

test_preset() {
    clean_build_dir

    pushd build

    build_preset "$1"

    echo "RUN TESTS IN PRESET = $1"
    ctest --output-on-failure
    check_exit_code

    popd # go back to root
}

run_tests_for_all_presets() {
    test_preset "default"
    test_preset "default release"
    test_preset "debug test static"
    test_preset "debug test shared"
    test_preset "release test static"
    test_preset "release test shared"
}

echo "Start running the test on all presets with /usr/bin/g++"
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
prompt_user_to_continue
if [ $? -eq 0 ]; then
    run_tests_for_all_presets
else
    echo "Skipped GCC"
fi

echo "Start running the test on all presets with /usr/bin/clang++"
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
prompt_user_to_continue
if [ $? -eq 0 ]; then
    run_tests_for_all_presets
else
    echo "Skipped Clang"
fi
