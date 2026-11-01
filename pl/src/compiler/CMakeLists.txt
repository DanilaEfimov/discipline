add_executable(derpy)
target_sources(derpy PRIVATE src/main.cpp)

target_link_libraries(pl_compiler INTERFACE pl::ir)
