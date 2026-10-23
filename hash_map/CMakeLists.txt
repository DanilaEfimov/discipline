cmake_minimum_required(VERSION 3.20)

project(hash_map_lib VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)		    # default variable value for every target
set(CMAKE_CXX_STANDARD_REQUIRED ON)	# no compiler version rollbacks
set(CMAKE_CXX_EXTENSIONS OFF)		# only c++ standard features, no gnu extensions

# generates compile_commands.json in build dir
# compile_commands.json needed by clangd, IDE and other analizers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# adding subdirectories section

add_subdirectory(src)	# entry subdir and execute nested CMakeLists.txt

enable_testing()
add_subdirectory(tests)

