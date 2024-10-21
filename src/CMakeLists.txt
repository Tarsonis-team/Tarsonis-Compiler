add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(analyzer)

add_executable(
    Tarsonis_Compiler
        main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Tarsonis_Compiler LEXER PARSER ANALYZER)
