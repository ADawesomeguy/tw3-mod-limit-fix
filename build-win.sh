#!/bin/bash
# Use MINGW for cross-compilation
x86_64-w64-mingw32-g++-win32 main.cpp modlimitfix.res -o modlimitfix.exe -std=c++17
