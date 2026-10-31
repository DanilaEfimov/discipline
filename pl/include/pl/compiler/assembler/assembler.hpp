#pragma once

#include "abi.hpp"
#include <ostream>

namespace pla {

class PlAssembler {
    std::ostream& out;

public:
    PlAssembler(std::ostream& out) : out(out) {}

    void write(plabi::Command operation) {
        this->out.write(reinterpret_cast<const char*>(&operation), plabi::command_size);
    }
};

} // namespace pla
