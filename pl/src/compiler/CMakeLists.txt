add_library(pl_compiler INTERFACE)
add_library(pl::compiler ALIAS pl_compiler)

target_link_libraries(pl_compiler INTERFACE pl::ir)
