#!/bin/bash
# Use MINGW for cross-compilation
x86_64-w64-mingw32-gcc-win32 main.cpp modlimitfix.res -lstdc++ -std=c++17
