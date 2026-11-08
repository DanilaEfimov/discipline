add_library(plpp INTERFACE)
target_include_directories(plpp INTERFACE
    $CMAKE_CURRENT_SOURCE_DIR/../include
)
target_link_libraries(plpp PRIVATE)
enable_testing()

add_executable(plpp_test plpp_test.cpp)
add_executable(tokenizer_test tokenizer_test.cpp)

add_test(NAME plpp_test COMMAND plpp_test)
add_test(NAME tokenizer_test COMMAND tokenizer_test)
