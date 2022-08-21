cmake_minimum_required(VERSION 3.7)
project(modlimitfix)

set(CMAKE_CXX_STANDARD 14)

add_executable(modlimitfix
        main.cpp)
target_link_libraries(modlimitfix -static-libgcc -static-libstdc++)
if (WIN32)
    message("WIN32")
    set_target_properties(modlimitfix PROPERTIES LINK_FLAGS ${CMAKE_CURRENT_SOURCE_DIR}/modlimitfix.res)
endif(WIN32)