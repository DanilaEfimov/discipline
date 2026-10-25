add_library(pl_vm INTERFACE)
add_library(pl::vm ALIAS pl_vm)

target_include_directories(pl_vm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(pl_ir INTERFACE pl::ir)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(plvm main.cpp)
target_link_libraries(plvm PRIVATE pl::vm Boost::program_options)
