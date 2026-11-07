#pragma once

#include "../diagnostics/diagnostic.hpp"
#include "../helper.hpp"
#include "symbols.hpp"
#include <istream>

namespace plpp {

class Plpp {

std::istream& source;

int lookahead = EOF;
bool has_lookahead = false;

std::size_t line = 1;
std::size_t column = 1;
std::size_t offset = 0;

  int raw_peek() {
    if(!this->has_lookahead){
      this->lookahead = this->source.peek();
      this->has_lookahead = true;
    }
    return this->lookahead;
  }

  int raw_get() {
    int ch = this->raw_peek();
    this->has_lookahead = false;
    
    if(ch == EOF) return EOF;
    this->source.get();

    if(ch == plc::ReservedSymbols::sym_eol) {
      this->line++;
      this->column = 0;
    }
    this->column++;
    this->offset++;

    return ch;
  }

  void skip_ignored() {
    for(;;) {
      int ch = this->raw_peek();
      if(ch == EOF) return;

      if(is_space(ch)) {
        ch = this->raw_get();
        continue;
      }

      if(ch == plc::ReservedSymbols::sym_semicolumn) {
        while(this->raw_peek() != plc::ReservedSymbols::sym_eol && this->raw_peek() != EOF)
          this->raw_get();
        continue;
      }

      return;
    }
  }

public:
  explicit Plpp(std::istream& in) : source(in) {}

  int peek() {
    this->skip_ignored();
    return this->raw_peek();
  }

  int get() {
    this->skip_ignored();
    return this->raw_get();
  }

  plc::LocalPosition current_position() const {
    return { this->line, this->column };
  }

  std::size_t current_offset() const {
    return this->offset;
  }

  bool eof() {
    return this->peek() == EOF;
  }

};

} // namespace plpp
