include(FetchContent)

    FetchContent_Declare(
        googletest URL
            https : // github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE) FetchContent_MakeAvailable(googletest)

#general crud test registration
        add_executable(test_hash_map test_hash_map.cpp) #a target
    target_link_libraries(test_hash_map PRIVATE hash_map::lib GTest::gtest_main)
        set_target_properties(test_hash_map PROPERTIES CXX_CLANG_TIDY "")
            add_test(NAME CRUD COMMAND test_hash_map)

#comparing with stdd::unordered_map(just for fun)
                add_executable(bench_mark bench_mark.cpp) #a target
    target_link_libraries(bench_mark PRIVATE hash_map::lib GTest::gtest_main)
        set_target_properties(bench_mark PROPERTIES CXX_CLANG_TIDY "")
            add_test(NAME BENCHMARK COMMAND bench_mark)
