#pragma once

#include "../helper.hpp"
#include "../keywords.hpp"
#include "../preprocessor/plpp.hpp"
#include "literal.hpp"
#include "token.hpp"
#include <cstdint>
#include <format>
#include <istream>

namespace plc {

class Tokenizer {
  plpp::Plpp &source;
  fileid_t file;

public:
  Tokenizer(plpp::Plpp &in, fileid_t file) : source(in), file(file) {}

  Token next() {
    Token token;

    this->source.peek();
    GlobalPosition current_position = {this->source.current_position(),
                                       this->file};
    int ch = this->source.get();

    switch (ch) {
    case EOF:
      return Token{"<EOF>", current_position, TokenCategory::Eof};

    case ReservedSymbols::sym_eol:
      return Token{std::format("{}", ReservedSymbols::sym_eol),
                   current_position, TokenCategory::Eol};

    case ReservedSymbols::sym_space:
      return Token{std::format("{}", ReservedSymbols::sym_space),
                   current_position, TokenCategory::Space};

    case ReservedSymbols::sym_column:
      return Token{std::format("{}", ReservedSymbols::sym_column),
                   current_position, TokenCategory::Column};

    case ReservedSymbols::sym_comma:
      return Token{std::format("{}", ReservedSymbols::sym_comma),
                   current_position, TokenCategory::Comma};

    case ReservedSymbols::sym_period:
      return Token{std::format("{}", ReservedSymbols::sym_period),
                   current_position, TokenCategory::Period};

    case ReservedSymbols::sym_semicolumn:
      // ! now it's a comment start symbol - ever ignored
      return Token{std::format("{}", ReservedSymbols::sym_semicolumn),
                   current_position, TokenCategory::Semicolumn};

    case ReservedSymbols::sym_directive:
      return Token{std::format("{}", ReservedSymbols::sym_directive),
                   current_position, TokenCategory::Directive};

    case ReservedSymbols::sym_lbracket:
      return Token{std::format("{}", ReservedSymbols::sym_lbracket),
                   current_position, TokenCategory::lBracket};

    case ReservedSymbols::sym_rbracket:
      return Token{std::format("{}", ReservedSymbols::sym_rbracket),
                   current_position, TokenCategory::rBracket};

    case ReservedSymbols::sym_lfbracket:
      return Token{std::format("{}", ReservedSymbols::sym_lfbracket),
                   current_position, TokenCategory::lfBracket};

    case ReservedSymbols::sym_rfbracket:
      return Token{std::format("{}", ReservedSymbols::sym_rfbracket),
                   current_position, TokenCategory::rfBracket};

    case ReservedSymbols::sym_lsbracket:
      return Token{std::format("{}", ReservedSymbols::sym_lsbracket),
                   current_position, TokenCategory::lsBracket};

    case ReservedSymbols::sym_rsbracket:
      return Token{std::format("{}", ReservedSymbols::sym_rsbracket),
                   current_position, TokenCategory::rsBracket};

    case ReservedSymbols::sym_zero:
      // start of a number literal
      {
        int next_ch = this->source.peek();
        LiteralPrefix prefix = resolve_prefix(static_cast<char>(next_ch));

        bool (*predicate)(char);
        switch (prefix) {
        case LiteralPrefix::Bin:
          predicate = TokenRecognizer::is_digit_bin;
          break;
        case LiteralPrefix::Hex:
          predicate = TokenRecognizer::is_digit_hex;
          break;
        case LiteralPrefix::Dec:
          predicate = TokenRecognizer::is_digit_dec;
          break;
        default:
          predicate = nullptr;
        }

        if (!predicate)
          return Token{std::format("{}{}", ReservedSymbols::sym_zero,
                                   prefix_to_char(prefix)),
                       current_position, TokenCategory::Unknown};

        this->source.get();

        std::string digits = "";
        int p;
        while ((p = this->source.peek()) != EOF &&
               predicate(static_cast<char>(p))) {
          ch = this->source.get();
          digits += static_cast<char>(ch);
        }

        if (digits.empty())
          return Token{std::format("{}{}", ReservedSymbols::sym_zero,
                                   prefix_to_char(prefix)),
                       current_position, TokenCategory::Unknown};

        return Token{std::format("{}{}{}", ReservedSymbols::sym_zero,
                                 prefix_to_char(prefix), digits),
                     current_position, TokenCategory::Literal};
      }

    default:
      // identifiers/keywords resolving
      if (!TokenRecognizer::is_ident_char(static_cast<char>(ch)))
        return Token{std::format("'{}' ({:#x})", static_cast<char>(ch),
                                 static_cast<unsigned>(ch)),
                     current_position, TokenCategory::Unknown};

      std::string word = "";
      word += static_cast<char>(ch);
      while (TokenRecognizer::is_ident_char(this->source.peek())) {
        ch = this->source.get();
        word += static_cast<char>(ch);
      }

      auto keyword_flag = is_keyword(word);
      TokenCategory category = keyword_flag == not_a_keyword
                                   ? TokenCategory::Identifier
                                   : TokenCategory::Keyword;

      return Token{word, current_position, category};
    }
  }
};

} // namespace plc
