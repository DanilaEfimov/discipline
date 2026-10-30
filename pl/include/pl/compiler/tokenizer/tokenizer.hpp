#pragma once

#include "helper.hpp"
#include "token.hpp"
#include "../keywords.hpp"
#include <cstdint>
#include <istream>

namespace plc {

class TokenizerContext {
  tokensize length;
};

class Tokenizer {
  std::istream &source;
  TokenizerContext context;
  linen_t pos = 0;
  fileid_t file;

  void skip_whitespace() {
    while(std::isspace(static_cast<unsigned char>(this->source.peek()))) {
      this->source.get();
      this->pos++;
    }
  }

public:
  Tokenizer(std::istream &in, fileid_t file) : source(in), file(file) {}

  Token next() {
    this->skip_whitespace();

    int ch;
    if ((ch = this->source.get()) == std::char_traits<char>::eof())
      return Token{GlobalPosition{this->pos, this->file}, TokenCategory::Eof, 0};

    this->pos = source.tellg();

    char current = source.get();
    Token token;
    TokenCategory category = TokenCategory::Unknown;
    tokensize length = 0;
    tokensize offset;

    switch(current) {
      case ReservedSymbols::sym_zero: // * number literal
      {
        char count_sys = this->source.get();
        switch(count_sys) {
          case 'd': // * decimal number
            offset = TokenRecognizer::try_literal_dec(this->source);
            if(offset == TokenRecognizer::no_match)
              break;
            length = offset;
          break;
          case 'x': // * heximal number
            offset = TokenRecognizer::try_literal_hex(this->source);
            if(offset == TokenRecognizer::no_match)
              break;
            length = offset;
          break;
          case 'b': // * binary number
            offset = TokenRecognizer::try_literal_bin(this->source);
            if(offset == TokenRecognizer::no_match)
              break;
            length = offset;
          break;
          default:
            category = TokenCategory::Unknown;
            length = 0;
        }
      }
      break;
      
      case ReservedSymbols::sym_directive: // * preprocessor directive
        offset = TokenRecognizer::try_directive(this->source);
        break;  // '$'
      case ReservedSymbols::sym_comma:
        offset = TokenRecognizer::try_comma(this->source);
        break;  // ','
      case ReservedSymbols::sym_column:     
        offset = TokenRecognizer::try_column(this->source);
        break;  // ':'
      case ReservedSymbols::sym_period:     
        offset = TokenRecognizer::try_period(this->source);
        break;  // '.'
      case ReservedSymbols::sym_semicolumn: 
        offset = TokenRecognizer::try_semicolumn(this->source);
        break;  // ';'
      case ReservedSymbols::sym_lbracket:   
        offset = TokenRecognizer::try_lbracket(this->source);
        break;  // '('
      case ReservedSymbols::sym_rbracket:   
        offset = TokenRecognizer::try_rbracket(this->source);
        break;  // ')'
      case ReservedSymbols::sym_lfbracket:  
        offset = TokenRecognizer::try_lfbracket(this->source);
        break;  // '{'
      case ReservedSymbols::sym_rfbracket:  
        offset = TokenRecognizer::try_rfbracket(this->source);
        break;  // '}'
      case ReservedSymbols::sym_lsbracket:  
        offset = TokenRecognizer::try_lsbracket(this->source);
        break;  // '['
      case ReservedSymbols::sym_rsbracket:  
        offset = TokenRecognizer::try_rsbracket(this->source);
        break;  // ']'

      default:
        if(TokenRecognizer::is_ident_char(current)) {
          std::string word;
          TokenRecognizer::try_identifier(this->source, word);

          int kw_index = is_keyword(word);
          category = (kw_index == not_a_keyword) 
            ? TokenCategory::Identifier 
            : TokenCategory::Keyword;

          this->pos += word.size();
          length = word.size();
        }
      break;
    }

    token = Token{GlobalPosition{this->pos, this->file}, category, length};

    return token;
  }
};

} // namespace plc
