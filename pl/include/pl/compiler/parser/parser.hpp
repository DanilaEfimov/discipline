#pragma once

#include "../tokenizer/token.hpp"
#include "ast.hpp"
#include "literal.hpp"
#include "symbol_table.hpp"
#include "token_stream.hpp"
#include <optional>
#include <vector>

namespace plp {

class PlParser {
  SymbolTable symbols;
  TokenStream &source;
  std::vector<plc::Token> line;
  std::vector<plc::Diagnostic> diags;
  std::size_t pos;
  plc::fileid_t file;
  bool eof_seen = false;

  void fill_line() {
    this->line.clear();
    while (true) {
      auto t = this->source.next();
      if (t.category == plc::TokenCategory::Eol)
        return;
      if (t.category == plc::TokenCategory::Eof) {
        this->eof_seen = true;
        return;
      }
      this->line.push_back(std::move(t));
    }
  }

  std::optional<Statement> parse_statement() {
    if (this->line.empty())
      return std::nullopt;
    if (this->line[0].category == plc::TokenCategory::Period)
      return this->parse_declaration();
    if (this->line[0].category == plc::TokenCategory::Keyword)
      return this->parse_operation();
    return std::nullopt;
  }

  std::optional<Declaration> parse_declaration() {
    Declaration decl;
    decl.span = this->line[0].position;

    std::size_t i = 1;

    if (i >= this->line.size() ||
        this->line[i].category != plc::TokenCategory::Identifier) {
      this->diags.push_back(
          plc::Diagnostic{plc::Severity::Error, plc::unexpected_token,
                          i < this->line.size() ? this->line[i].position
                                                : this->line[0].position,
                          std::format("expected identifier after '.'")});
      return std::nullopt;
    }
    decl.identigier = this->line[i++].lexeme;

    if (i < this->line.size()) {
      if (this->line[i].category != plc::TokenCategory::Literal) {
        this->diags.push_back(plc::Diagnostic{
            plc::Severity::Error, plc::unexpected_token, this->line[i].position,
            std::format("expected literal instead '{}'",
                        this->line[i].lexeme)});
        return std::nullopt;
      }
      try {
        decl.address = plp::parse_literal(this->line[i]);
      } catch (plc::Diagnostic &d) {
        this->diags.push_back(std::move(d));
        return std::nullopt;
      }
      ++i;
    }

    if (i < this->line.size()) {
      this->diags.push_back(plc::Diagnostic{
          plc::Severity::Error, plc::unexpected_token, this->line[i].position,
          std::format("unexpected token '{}' after declaration",
                      this->line[i].lexeme)});
      return std::nullopt;
    }

    return decl;
  }

  std::optional<Operation> parse_operation() {
    Operation op;

    if (this->line.empty()) {
      this->diags.push_back(plc::Diagnostic{plc::Severity::Error,
                                            plc::unexpected_end,
                                            {{}, this->file},
                                            "expected mnemonic"});
      return std::nullopt;
    }

    op.span = this->line[0].position;

    auto kw = plc::is_keyword(this->line[0].lexeme);
    if (!kw) {
      this->diags.push_back(plc::Diagnostic{
          plc::Severity::Error, plc::unexpected_token, this->line[0].position,
          std::format("expected mnemonic instead '{}'", this->line[0].lexeme)});
      return std::nullopt;
    }
    op.keyword_id = kw;

    std::size_t i = 1;

    if (i < this->line.size()) {
      auto e = this->parse_expression(this->line, i);
      if (!e)
        return std::nullopt;
      op.arg1 = std::move(*e);
    }

    if (i < this->line.size()) {
      auto e = this->parse_expression(this->line, i);
      if (!e)
        return std::nullopt;
      op.arg2 = std::move(*e);
    }

    if (i < this->line.size()) {
      this->diags.push_back(plc::Diagnostic{
          plc::Severity::Error, plc::unexpected_token, this->line[i].position,
          std::format("unexpected token '{}' after operands",
                      this->line[i].lexeme)});
      return std::nullopt;
    }

    return op;
  }

  std::optional<Expression>
  parse_expression(const std::vector<plc::Token> &tokens, std::size_t &pos) {
    const plc::Token &t = tokens[pos];

    Expression e;
    e.span = t.position;

    if (t.category == plc::TokenCategory::Identifier) {
      e.type = plp::ExpressionType::Identifier;
      e.identifier = t.lexeme;
      ++pos;
      return e;
    }

    if (t.category == plc::TokenCategory::Literal) {
      e.type = plp::ExpressionType::Literal;
      try {
        e.value = plp::parse_literal(t);
      } catch (plc::Diagnostic &d) {
        this->diags.push_back(std::move(d));
        return std::nullopt;
      }
      ++pos;
      return e;
    }

    if (t.category == plc::TokenCategory::lsBracket) {
      if (pos + 2 >= tokens.size() ||
          tokens[pos + 1].category != plc::TokenCategory::Literal ||
          tokens[pos + 2].category != plc::TokenCategory::rsBracket) {
        this->diags.push_back(
            plc::Diagnostic{plc::Severity::Error, plc::unexpected_token,
                            t.position, std::format("expected '[literal]'")});
        return std::nullopt;
      }
      e.type = plp::ExpressionType::Address;
      try {
        e.value = plp::parse_literal(tokens[pos + 1]);
      } catch (plc::Diagnostic &d) {
        this->diags.push_back(std::move(d));
        return std::nullopt;
      }
      pos += 3;
      return e;
    }

    this->diags.push_back(plc::Diagnostic{
        plc::Severity::Error, plc::unexpected_token, t.position,
        std::format("expected identifier, literal or '[literal]', got '{}'",
                    t.lexeme)});
    return std::nullopt;
  }

public:
  PlParser(TokenStream &source, plc::fileid_t file)
      : source(source), file(file) {};

  std::optional<plp::Statement> next() {
    if (this->eof_seen)
      return std::nullopt;
    while (true) {
      this->fill_line();
      if (this->line.empty()) {
        if (this->eof_seen)
          return std::nullopt;
        continue;
      }
      this->pos = 0;
      return this->parse_statement();
    }
  }

  bool finished() { return this->eof_seen; }

  const std::vector<plc::Diagnostic> &get_diagnostics() { return this->diags; }
};

} // namespace plp
