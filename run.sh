#!/usr/bin/fish
cmake -S . -B build -G Ninja
cmake --build build/ -j16
./build/main
