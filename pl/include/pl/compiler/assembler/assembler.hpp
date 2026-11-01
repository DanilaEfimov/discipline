#pragma once

#include "../parser/ast.hpp"
#include "abi.hpp"
#include <ostream>

namespace pla {

class PlAssembler {
  std::ostream &out;

public:
  PlAssembler(std::ostream &out) : out(out) {}

  void write(plp::Statement statement) {
    // TODO: implement
  }
};

} // namespace pla
