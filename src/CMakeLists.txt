add_subdirectory(lexer)

add_executable(
    Tarsonis_Compiler
        main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Tarsonis_Compiler LEXER)
