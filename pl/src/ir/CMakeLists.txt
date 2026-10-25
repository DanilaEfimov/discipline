add_library(pl_ir INTERFACE)
add_library(pl::ir ALIAS pl_ir)

target_include_directories(pl_ir INTERFACE
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_features(pl_ir INTERFACE cxx_std_20)
