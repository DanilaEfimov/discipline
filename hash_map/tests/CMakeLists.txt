include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(test_hash_map test_hash_map.cpp)
target_link_libraries(test_hash_map PRIVATE hash_map::lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_hash_map)

set_target_properties(test_hash_map PRIVATE CXX_CLANG_TIGY "")

