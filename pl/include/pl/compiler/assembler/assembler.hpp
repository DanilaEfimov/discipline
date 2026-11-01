#pragma once

#include "abi.hpp"
#include "../parser/ast.hpp"
#include <ostream>

namespace pla {

class PlAssembler {
    std::ostream& out;

public:
    PlAssembler(std::ostream& out) : out(out) {}

    void write(plp::Statement statement) {
        // TODO: implement
    }
};

} // namespace pla
