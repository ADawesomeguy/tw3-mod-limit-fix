#!/bin/bash
mkdir -p bin/
g++ main.cpp -o bin/modlimitfix -std=c++17 -lstdc++ -static
