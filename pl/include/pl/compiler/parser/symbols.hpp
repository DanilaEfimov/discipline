#pragma once

#include "../assembler/abi.hpp"
#include "../diagnostics/diagnostic.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace plp {

enum class OverwritePolicy {
    Error,
    Overwrite
};

using symbol_t = std::string;
using value_t = plabi::address_t;

struct SymbolTableConfig {
    std::unordered_map<symbol_t, value_t> predefined;
    OverwritePolicy overwrite_policy = OverwritePolicy::Overwrite;
    bool case_sensitive = true;
    std::uint8_t max_identifier_length = 0xFF;
};

class SymbolTable {

    std::unordered_map<symbol_t, value_t> defined_symbols;
    std::unordered_set<symbol_t> forward_declarations;
    SymbolTableConfig config;

    bool check_symbol(const symbol_t& symbol, value_t value, plc::Diagnostic& what) {

    }

public:
    SymbolTable(SymbolTableConfig cfg)
        : defined_symbols(), config(cfg) {
            this->defined_symbols.emplace(std::move(cfg.predefined));
        }

    bool define_symbol(const symbol_t& symbol, value_t value, plc::Diagnostic& what) {
        bool ok = this->check_symbol(symbol, value, what);
        if(!ok) return false;

        this->defined_symbols[symbol] = value;
        return this->defined_symbols.contains(symbol);
    }

    bool define_forward(const symbol_t& symbol, plc::Diagnostic& what) {

    }

    bool has_forward_declared_symbols() const noexcept {
        return this->forward_declarations.empty();
    }
};

} // namespace plp
