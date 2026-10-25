add_library(pl_vm INTERFACE)
add_library(pl::vm ALIAS pl_vm)

target_link_libraries(pl_ir INTERFACE pl::ir)
