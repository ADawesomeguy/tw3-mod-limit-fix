#!/bin/bash
mkdir -p bin/
# Use MINGW for cross-compilation
x86_64-w64-mingw32-g++ main.cpp -o bin/modlimitfix.exe -std=c++17 --static
