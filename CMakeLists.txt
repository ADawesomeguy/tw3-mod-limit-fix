cmake_minimum_required(VERSION 3.12)
project(modlimitfix)

set(CMAKE_CXX_STANDARD 17)

add_executable(modlimitfix
        main.cpp)
target_link_libraries(modlimitfix -static-libgcc -static-libstdc++)
