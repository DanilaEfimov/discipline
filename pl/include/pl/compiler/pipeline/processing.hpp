#pragma once

#include "../assembler/assembler.hpp"
#include "../parser/parser.hpp"
#include "../preprocessor/plpp.hpp"
#include "../tokenizer/tokenizer.hpp"

namespace plc {

class Processing {
  pla::PlAssembler &assembler;
  plp::PlParser &parser;

public:
  Processing(plp::PlParser &p, pla::PlAssembler &a) : assembler(a), parser(p) {}

  const std::vector<Diagnostic> &run() {
    while (!this->parser.finished()) {
      auto statement = this->parser.next();
      if (statement.has_value())
        this->assembler.write(statement.value());
      else
        break;
    }
    return this->parser.get_diagnostics();
  }
};

} // namespace plc
