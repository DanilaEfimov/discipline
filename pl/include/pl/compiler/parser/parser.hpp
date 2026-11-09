#pragma once

#include "../diagnostics/diagnostic.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "ast.hpp"
#include "bnf.hpp"
#include "inspector.hpp"
#include "semantics.hpp"
#include "symbols.hpp"
#include <vector>

namespace plp {

class Parser {
  plc::Tokenizer &tokenizer;
  SymbolTable symbol_table;
  Inspector inspector;
  program_t program;

  std::vector<plc::Token> line;
  bool eof_seen = false;

  void fill_line() {
    this->line.clear();
    plc::Token last = this->tokenizer.next();
    while (last.category != plc::TokenCategory::Eol) {
      if (last.category == plc::TokenCategory::Eof) {
        this->eof_seen = true;
        return;
      }
      this->line.push_back(std::move(last));
    }
  }

  std::optional<Declaration> parse_declaration() {
    // TODO: implement
    plc::Diagnostic what;
    bool is_correct_sentence = BnfTree::definition_match(this->line, RuleRepo::RuleName::Declaration, what);

    if(!is_correct_sentence) {
      this->inspector.note(what);
      return std::nullopt;
    }

    Declaration declaration;
    plc::Token symbol = this->line[1];
    plc::Token address_arg = this->line[2];
  }

  std::optional<Operation> parse_operation() {
    // TODO: implement
    plc::Diagnostic what;
    bool is_correct_sentence = BnfTree::definition_match(this->line, RuleRepo::RuleName::Operation, what);

    if(!is_correct_sentence) {
      this->inspector.note(what);
      return std::nullopt;
    }
  }

  std::optional<statement_t> parse_line() {
    plc::Diagnostic what;
    StatementType type = StatementRecognizer::recognize(this->line, what);
    switch (type) {
    case StatementType::Declaration:
      return this->parse_declaration();
    case StatementType::Operation:
      return this->parse_operation();
    default:
      this->inspector.note(what);
      return std::nullopt;
    }
  }

public:
  explicit Parser(plc::Tokenizer &tk, InspectorConfig icfg, SymbolTableConfig tcfg)
      : tokenizer(tk), inspector(icfg), symbol_table(tcfg) {}

  void parse_program() {
    while (!this->finished()) {
      this->parse_next();
    }
  }

  void parse_next() {
    this->fill_line();

    auto statement = this->parse_line();
    if (statement.has_value()) {
      StatementType type = static_cast<StatementType>(statement->index());
      switch (type) {
      case StatementType::Declaration:
        // TODO: symbol table management
        // this->program.push_back(
        //     std::move(std::get<Declaration>(statement.value())));
        break;

      case StatementType::Operation:
        this->program.push_back(
            std::move(std::get<Operation>(statement.value())));
        break;

      default:
        break;
      }
    }
  }

  const program_t &get_program() const { return this->program; }

  bool finished() const { return this->eof_seen; }

  const std::vector<plc::Diagnostic> &get_diagnostics() const {
    return this->inspector.get_diagnostics();
  }
};

} // namespace plp
