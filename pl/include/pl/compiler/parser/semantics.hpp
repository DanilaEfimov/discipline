#pragma once

#include "ast.hpp"
#include "../tokenizer/token.hpp"
#include "../diagnostics/diagnostic.hpp"
#include <span>

namespace plp {

// * 8 is a upper bound of statement length in tokens
struct BnfDefinition {
    std::span<plc::TokenCategory> pattern;
    StatementType type;
};

inline constexpr std::size_t declaration_variants = 3;
std::array<BnfDefinition, declaration_variants> declaration_definitions = {

};

inline constexpr std::size_t operation_variants = 3;
std::array<BnfDefinition, operation_variants> operation_definitions = {

};

struct StatementRecognizer {

static bool declaration_match(const std::vector<plc::Token>& line, plc::Diagnostic& what) {

}

static bool operation_match(const std::vector<plc::Token>& line, plc::Diagnostic& what) {

}

static StatementType recognize(const std::vector<plc::Token>& line, plc::Diagnostic& what) {
    if(declaration_match(line, what)) return StatementType::Declaration;
    if(operation_match(line, what)) return StatementType::Operation;
    return StatementType::None;
}

};

} // namespace plp
