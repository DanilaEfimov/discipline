add_library(plpp INTERFACE)
target_include_directories(plpp INTERFACE
    $CMAKE_CURRENT_SOURCE_DIR/../include
)

add_executable(plpp_test plpp_test.cpp)
add_executable(tekonizer_test tokenizer_test.cpp)
target_link_libraries(plpp PRIVATE)

enable_testing()
add_test(NAME plpp_test COMMAND plpp_test)
add_test(NAME token_test COMMAND tokenizer_test)
