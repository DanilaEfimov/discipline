cmake_minimum_required(VERSION 3.20)

project(pl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/ir)
add_subdirectory(src/vm)
add_subdirectory(src/compiler)

include(CTest)  # provides enable_testing() with BUILD_TESTING option
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
