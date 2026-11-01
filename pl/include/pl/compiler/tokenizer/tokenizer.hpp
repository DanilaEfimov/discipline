#pragma once

#include "../helper.hpp"
#include "../keywords.hpp"
#include "../preprocessor/plpp.hpp"
#include "token.hpp"
#include <cstdint>
#include <istream>

namespace plc {

class TokenizerContext {
  tokensize length;
};

class Tokenizer {
  plpp::Plpp &source;
  TokenizerContext context;
  fileid_t file;

public:
  Tokenizer(plpp::Plpp &in, fileid_t file) : source(in), file(file) {}

  Token next() {
    int ch = this->source.peek();
    offset_t start_pos = this->source.get_current_position().local.offset;
    if (this->source.tellg() == std::streampos(-1))
      return Token{
          "",
          GlobalPosition{{this->source.get_current_position().local.offset},
                         this->file},
          TokenCategory::Eof};

    offset_t start = static_cast<offset_t>(start_pos);

    if (ch == std::char_traits<char>::eof())
      return Token{"", GlobalPosition{{start}, this->file}, TokenCategory::Eof};

    char current = static_cast<char>(ch);
    TokenCategory category = TokenCategory::Unknown;
    std::string lexeme;

    switch (current) {
    case ReservedSymbols::sym_zero: {
      this->source.get();
      int prefix = this->source.peek();
      if (prefix == 'd' || prefix == 'x' || prefix == 'b') {
        this->source.get();
        tokensize digits = 0;
        if (prefix == 'd')
          digits = TokenRecognizer::try_literal_dec(this->source, lexeme);
        else if (prefix == 'x')
          digits = TokenRecognizer::try_literal_hex(this->source, lexeme);
        else
          digits = TokenRecognizer::try_literal_bin(this->source, lexeme);

        if (digits != TokenRecognizer::no_match)
          category = TokenCategory::Literal;
        else
          category = TokenCategory::Unknown;
      } else
        category = TokenCategory::Unknown;
      break;
    }

    case ReservedSymbols::sym_oel:
      this->source.get();
      category = TokenCategory::Eol;
      lexeme = current;
      break;

    case ReservedSymbols::sym_directive:
      this->source.get();
      category = TokenCategory::Directive;
      lexeme = current;
      break;
    case ReservedSymbols::sym_comma:
      this->source.get();
      category = TokenCategory::Comma;
      lexeme = current;
      break;
    case ReservedSymbols::sym_column:
      this->source.get();
      category = TokenCategory::Column;
      lexeme = current;
      break;
    case ReservedSymbols::sym_period:
      this->source.get();
      category = TokenCategory::Period;
      lexeme = current;
      break;
    case ReservedSymbols::sym_semicolumn:
      this->source.get();
      category = TokenCategory::Semicolumn;
      lexeme = current;
      break;
    case ReservedSymbols::sym_lbracket:
      this->source.get();
      category = TokenCategory::lBracket;
      lexeme = current;
      break;
    case ReservedSymbols::sym_rbracket:
      this->source.get();
      category = TokenCategory::rBracket;
      lexeme = current;
      break;
    case ReservedSymbols::sym_lfbracket:
      this->source.get();
      category = TokenCategory::lfBracket;
      lexeme = current;
      break;
    case ReservedSymbols::sym_rfbracket:
      this->source.get();
      category = TokenCategory::rfBracket;
      lexeme = current;
      break;
    case ReservedSymbols::sym_lsbracket:
      this->source.get();
      category = TokenCategory::lsBracket;
      lexeme = current;
      break;
    case ReservedSymbols::sym_rsbracket:
      this->source.get();
      category = TokenCategory::rsBracket;
      lexeme = current;
      break;

    default:
      if (TokenRecognizer::is_ident_char(current)) {
        TokenRecognizer::try_identifier(this->source, lexeme);
        int kw = is_keyword(lexeme);
        category = (kw == not_a_keyword) ? TokenCategory::Identifier
                                         : TokenCategory::Keyword;
      } else {
        this->source.get();
        category = TokenCategory::Unknown;
      }
      break;
    }

    return Token{lexeme, GlobalPosition{{start}, this->file}, category};
  }
};

} // namespace plc
