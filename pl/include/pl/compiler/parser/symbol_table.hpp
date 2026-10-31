#pragma once

#include "../assembler/abi.hpp"
#include <string>
#include <unordered_map>

namespace plp {

struct Symbol {
    std::string identifier;
    plabi::address_t address;
};

class SymbolTable {
    std::unordered_map<std::string, plabi::address_t> symbols;
};

} // namespace plp
