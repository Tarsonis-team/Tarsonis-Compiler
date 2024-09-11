
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(
    LEXER STATIC 
            lexer.cpp
            token.cpp
            lexer-controller.cpp
    )
