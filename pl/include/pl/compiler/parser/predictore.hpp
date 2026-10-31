#pragma once

#include "semantics.hpp"
#include "../tokenizer/token.hpp"
#include <vector>

namespace plp {

class Predictor {
public:
    static StatementType predict(const std::vector<plc::Token>& tokens) {
        
        return StatementType::Unknown;
    }
};

} // namespace plp
