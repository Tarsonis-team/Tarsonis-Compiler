cmake_minimum_required(VERSION 3.20)
project(Tarsonis_Compiler)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(tests)
