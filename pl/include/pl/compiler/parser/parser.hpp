#pragma once

#include "../tokenizer/token.hpp"
#include "semantics.hpp"
#include <vector>

namespace plp {

struct ParserContext {
    StatementType waiting_for;
};

class PlParser {
    std::vector<plc::Token> buffer;
    ParserContext context;

public:



};

} // namespace plp
