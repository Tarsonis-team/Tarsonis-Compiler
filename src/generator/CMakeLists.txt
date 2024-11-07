include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(
        GENERATOR STATIC
        generator.cpp
)

set(LLVM_DIR "C:/Program Files/LLVM/lib/cmake/llvm")
set(CMAKE_PREFIX_PATH "C:/Program Files/LLVM/lib/cmake/llvm" CACHE STRING "LLVM installation path")

find_package(LLVM REQUIRED CONFIG)

target_include_directories(GENERATOR PRIVATE ${CMAKE_SOURCE_DIR}/src ${LLVM_INCLUDE_DIRS})
target_link_libraries(GENERATOR LLVM)
