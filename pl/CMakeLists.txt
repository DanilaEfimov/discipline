cmake_minimum_required(3.20)

project(pl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PL_BUILD_TESTS "Build tests" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory(src/ir)
add_subdirectory(src/vm)
add_subdirectory(src/compiler)

if(PL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
