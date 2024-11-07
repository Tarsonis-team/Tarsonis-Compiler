add_library(
        GENERATOR STATIC
        generator.cpp
)

target_include_directories(GENERATOR PRIVATE 
                        ${CMAKE_CURRENT_SOURCE_DIR} 
                        ${CMAKE_SOURCE_DIR}/src)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# llvm_map_components_to_libnames(llvm_libs core irreader)
target_link_libraries(GENERATOR ${llvm_libs})

