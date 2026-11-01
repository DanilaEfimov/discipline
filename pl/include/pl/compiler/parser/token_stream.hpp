#pragma once

#include "../tokenizer/token.hpp"

namespace plp {

class TokenStream {
public:
  virtual ~TokenStream() = default;
  virtual const plc::Token &next() = 0;
};

} // namespace plp
