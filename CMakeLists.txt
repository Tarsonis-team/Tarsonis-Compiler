cmake_minimum_required(VERSION 3.23)
project(Tarsonis_Compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(Tarsonis_Compiler
        src/lexer/main.cpp
        src/lexer/token-lib.cpp)
