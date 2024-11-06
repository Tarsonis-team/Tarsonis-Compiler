
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(
    PARSER STATIC 
    parser.cpp
)

target_include_directories(PARSER PRIVATE ${CMAKE_SOURCE_DIR}/src)
