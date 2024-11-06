include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(
        GENERATOR STATIC
        generator.cpp
)

target_include_directories(GENERATOR PRIVATE ${CMAKE_SOURCE_DIR}/src)
