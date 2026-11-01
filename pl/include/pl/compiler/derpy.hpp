#pragma once

#include "pipeline/processing.hpp"

namespace plc {

class PlCompiler {
    plc::Processing& processing;

public:
    PlCompiler(Processing& p): processing(p) {}

    const std::vector<plc::Diagnostic>& compile() {
        return this->processing.run();
    }
};

} // namespace plc
