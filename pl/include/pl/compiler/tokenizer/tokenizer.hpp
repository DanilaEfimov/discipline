#pragma once

#include "../helper.hpp"
#include "../keywords.hpp"
#include "../preprocessor/plpp.hpp"
#include "token.hpp"
#include <cstdint>
#include <istream>

namespace plc {

class Tokenizer {
  plpp::Plpp &source;
  fileid_t file;

public:
  Tokenizer(plpp::Plpp &in, fileid_t file) : source(in), file(file) {}

  Token next() {}
};

} // namespace plc
