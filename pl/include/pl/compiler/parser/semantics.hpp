#pragma once

#include "ast.hpp"
#include "bnf.hpp"
#include "../tokenizer/token.hpp"
#include "../diagnostics/diagnostic.hpp"

namespace plp {

using namespace pl::bnf;

struct StatementRecognizer {

static bool declaration_match(const std::vector<plc::Token>& line, plc::Diagnostic& what) {
    return definition_match(line, Rule{RuleRepo::operation_terms}, what);
}

static bool operation_match(const std::vector<plc::Token>& line, plc::Diagnostic& what) {
    return definition_match(line, Rule{RuleRepo::declaration_terms}, what);
}

static StatementType recognize(const std::vector<plc::Token>& line, plc::Diagnostic& what) {
    if(declaration_match(line, what)) return StatementType::Declaration;
    if(operation_match(line, what)) return StatementType::Operation;
    return StatementType::None;
}

};

} // namespace plp
