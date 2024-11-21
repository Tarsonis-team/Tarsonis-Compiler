add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(analyzer)
add_subdirectory(generator)

add_executable(
    Tarsonis_Compiler
        main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core irreader)
target_link_libraries(Tarsonis_Compiler LEXER PARSER ANALYZER GENERATOR ${llvm_libs})
# target_link_libraries(Tarsonis_Compiler LEXER PARSER ANALYZER GENERATOR)
