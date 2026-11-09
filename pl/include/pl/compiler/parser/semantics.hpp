#pragma once

#include "../diagnostics/diagnostic.hpp"
#include "../tokenizer/token.hpp"
#include "ast.hpp"
#include "bnf.hpp"

namespace plp {

using namespace pl::bnf;

struct StatementRecognizer {

  static bool declaration_match(const std::vector<plc::Token> &line,
                                plc::Diagnostic &what) {
    return BnfTree::definition_match(line, RuleRepo::RuleName::Declaration,
                                     what);
  }

  static bool operation_match(const std::vector<plc::Token> &line,
                              plc::Diagnostic &what) {
    return BnfTree::definition_match(line, RuleRepo::RuleName::Operation, what);
  }

  static StatementType recognize(const std::vector<plc::Token> &line,
                                 plc::Diagnostic &what) {
    if (declaration_match(line, what))
      return StatementType::Declaration;
    if (operation_match(line, what))
      return StatementType::Operation;
    return StatementType::None;
  }
};

} // namespace plp
