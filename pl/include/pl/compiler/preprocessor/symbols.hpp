#pragma once

#include "../helper.hpp"

namespace plpp {

inline constexpr char start_of_comment = plc::ReservedSymbols::sym_semicolumn;
inline constexpr bool is_space(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\b';
};

} // namespace plpp
