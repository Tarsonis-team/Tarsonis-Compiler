include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(
    ANALYZER STATIC 
    analyzer.cpp
    )

target_include_directories(ANALYZER PRIVATE ${CMAKE_SOURCE_DIR}/src)
