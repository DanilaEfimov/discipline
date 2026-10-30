#pragma once

#include "../keywords.hpp"
#include "helper.hpp"
#include "token.hpp"
#include <cstdint>
#include <istream>

namespace plc {

class TokenizerContext {
  tokensize length;
};

class Tokenizer {
  std::istream &source;
  TokenizerContext context;
  offset_t pos = 0;
  fileid_t file;

  void skip_whitespace() {
    int c = this->source.peek();
    while (std::isspace(static_cast<unsigned char>(c)) &&
           c != std::char_traits<char>::eof() &&
           c != ReservedSymbols::sym_oel) {
      this->source.get();
      this->pos++;
      c = this->source.peek();
    }
  }

public:
  Tokenizer(std::istream &in, fileid_t file) : source(in), file(file) {}

  Token next() {
    this->skip_whitespace();

    std::streampos start_pos = this->source.tellg();
    if (start_pos == std::streampos(-1))
      return Token{GlobalPosition{{this->pos}, this->file}, TokenCategory::Eof,
                   0};

    offset_t start = static_cast<offset_t>(start_pos);

    int ch = this->source.peek();
    if (ch == std::char_traits<char>::eof())
      return Token{GlobalPosition{{start}, this->file}, TokenCategory::Eof, 0};

    char current = static_cast<char>(ch);
    TokenCategory category = TokenCategory::Unknown;
    tokensize length = 0;

    switch (current) {
    case ReservedSymbols::sym_zero: {
      this->source.get();
      int prefix = this->source.peek();
      if (prefix == 'd' || prefix == 'x' || prefix == 'b') {
        this->source.get();
        tokensize digits = 0;
        if (prefix == 'd')
          digits = TokenRecognizer::try_literal_dec(this->source);
        else if (prefix == 'x')
          digits = TokenRecognizer::try_literal_hex(this->source);
        else
          digits = TokenRecognizer::try_literal_bin(this->source);

        if (digits != TokenRecognizer::no_match) {
          category = TokenCategory::Literal;
          length = static_cast<tokensize>(2 + digits); // '0' + prefix + digits
        } else {
          category = TokenCategory::Unknown;
          length = 2;
        }
      } else {
        category = TokenCategory::Unknown;
        length = 1;
      }
      break;
    }

    case ReservedSymbols::sym_oel:
      this->source.get();
      category = TokenCategory::Eol;
      length = 1;
      break;

    case ReservedSymbols::sym_directive:
      this->source.get();
      category = TokenCategory::Directive;
      length = 1;
      break;
    case ReservedSymbols::sym_comma:
      this->source.get();
      category = TokenCategory::Comma;
      length = 1;
      break;
    case ReservedSymbols::sym_column:
      this->source.get();
      category = TokenCategory::Column;
      length = 1;
      break;
    case ReservedSymbols::sym_period:
      this->source.get();
      category = TokenCategory::Period;
      length = 1;
      break;
    case ReservedSymbols::sym_semicolumn:
      this->source.get();
      category = TokenCategory::Semicolumn;
      length = 1;
      break;
    case ReservedSymbols::sym_lbracket:
      this->source.get();
      category = TokenCategory::lBracket;
      length = 1;
      break;
    case ReservedSymbols::sym_rbracket:
      this->source.get();
      category = TokenCategory::rBracket;
      length = 1;
      break;
    case ReservedSymbols::sym_lfbracket:
      this->source.get();
      category = TokenCategory::lfBracket;
      length = 1;
      break;
    case ReservedSymbols::sym_rfbracket:
      this->source.get();
      category = TokenCategory::rfBracket;
      length = 1;
      break;
    case ReservedSymbols::sym_lsbracket:
      this->source.get();
      category = TokenCategory::lsBracket;
      length = 1;
      break;
    case ReservedSymbols::sym_rsbracket:
      this->source.get();
      category = TokenCategory::rsBracket;
      length = 1;
      break;

    default:
      if (TokenRecognizer::is_ident_char(current)) {
        std::string word;
        TokenRecognizer::try_identifier(this->source, word);
        int kw = is_keyword(word);
        category = (kw == not_a_keyword) ? TokenCategory::Identifier
                                         : TokenCategory::Keyword;
        length = static_cast<tokensize>(word.size());
      } else {
        this->source.get();
        category = TokenCategory::Unknown;
        length = 1;
      }
      break;
    }

    this->pos = static_cast<offset_t>(this->source.tellg());

    return Token{GlobalPosition{{start}, this->file}, category, length};
  }
};

} // namespace plc
