#pragma once

#include "../../ir/signatures.hpp"
#include "../tokenizer/tokenizer.hpp"

namespace plp {

using token_t = plc::Token;

class ArgsLine {
  std::span<const token_t> tokens;
  int seen;

public:
  ArgsLine(std::span<const token_t> tokens) : tokens(tokens), seen(0) {}

  /**
   * @brief searchs the next argument token
   * @param t token to write the current value
   * @return true if next exists
   * @return false otherwise
   */
  bool next(token_t &t) {
    for (int i = this->seen; i < this->tokens.size(); ++i) {
      const token_t &current = this->tokens[i];
      if (current.category != plc::TokenCategory::Identifier &&
          current.category != plc::TokenCategory::Literal)
        continue;
      t = current;
      this->seen = i;
      return true;
    }
    return false;
  }
};

inline pl::ValueCategory categorize_value(const token_t &token) {
  switch (token.category) {
  case plc::TokenCategory::Identifier:
    return pl::ValueCategory::Variable;
  case plc::TokenCategory::Literal:
    return pl::ValueCategory::Literal;
  default:
    return pl::ValueCategory::None;
  }
}

} // namespace plp
