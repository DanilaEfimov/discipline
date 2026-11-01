#pragma once

#include "../assembler/abi.hpp"
#include "../diagnostics/diagnostic.hpp"
#include "../diagnostics/errors.hpp"
#include <format>
#include <string>
#include <string_view>
#include <unordered_map>

namespace plp {

class SymbolTable {
    std::unordered_map<std::string, plabi::address_t> names;

public:
    void define(std::string name, plabi::address_t address,
                plc::GlobalPosition span) {
        auto [it, inserted] = names.try_emplace(std::move(name), address);
        if (!inserted) {
            throw plc::Diagnostic{
                plc::Severity::Error,
                plc::redefinition,
                span,
                std::format("redefinition of symbol '{}'", it->first),
                { std::format("previously defined at 0x{:x}", it->second) },
                {}
            };
        }
    }

    plabi::address_t resolve(std::string_view name,
                             plc::GlobalPosition span) const {
        auto it = names.find(std::string(name));
        if (it == names.end()) {
            throw plc::Diagnostic{
                plc::Severity::Error,
                plc::undefined_symbol,
                span,
                std::format("undefined symbol '{}'", name),
                {},
                {}
            };
        }
        return it->second;
    }

    bool contains(std::string_view name) const {
        return names.find(std::string(name)) != names.end();
    }

    std::size_t size() const noexcept { return names.size(); }
};

} // namespace plp
