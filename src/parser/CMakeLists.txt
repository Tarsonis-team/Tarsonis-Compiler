
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(
    PARSER STATIC
            parser.cpp
            tree.cpp
            node.cpp
    )
