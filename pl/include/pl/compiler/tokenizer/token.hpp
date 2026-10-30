#pragma once

#include <cstdint>

namespace plc {

enum class TokenCategory {
  Eof,        // enf of file
  Eol,        // end of line
  Unknown,    // unknown token
  Identifier,
  Keyword,    // see keywords.hpp
  Comma,      // ,
  Column,     // :
  Semicolumn, // ;
  Directive,  // $
  lBracket,   // (
  rBracket,   // )
  lfBracket,  // {
  rfBracket,  // }
  lsBracket,  // [
  rsBracket,  // ]
  Period,     // .
  Literal,    // hex | decimal
};

using linen_t = std::uint32_t;
using offset_t = std::uint32_t;

struct LocalPosition {
  linen_t line;
  offset_t offset;
};

using fileid_t = std::uint16_t;

struct GlobalPosition {
  LocalPosition local;
  fileid_t file_id;
};

using tokensize = std::uint8_t;

struct Token {
  GlobalPosition position;
  TokenCategory category;
  tokensize length;
};

} // namespace plc
