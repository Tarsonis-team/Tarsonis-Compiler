include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(TestSequenceBreaker test-sequence-breaker.cpp)
target_link_libraries(TestSequenceBreaker PRIVATE gtest gtest_main)

add_test(NAME TestSequenceBreaker COMMAND TestSequenceBreaker)

include_directories(${CMAKE_SOURCE_DIR}/src)
