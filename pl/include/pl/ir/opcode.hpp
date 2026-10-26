#pragma once

#include <cstdint>

namespace pl {

enum OpCode : uint8_t {
  End           = 0x00,
  AddLiteral    = 0x01,
  AddVar        = 0x02,
  MulLiteral    = 0x03,
  MulVar        = 0x04,
  DivLiteral    = 0x05,
  DivVar        = 0x06,
  EqualLiteral  = 0x07,
  EqualVar      = 0x08,
  LessLiteral   = 0x09,
  LessVar       = 0x0A,
  RightLiteral  = 0x0B,
  RightVar      = 0x0C,
  LeftLiteral   = 0x0D,
  LeftVar       = 0x0E,
  WriteLiteral  = 0x0F,
  WriteVar      = 0x10,
  Print         = 0x11,
  Read          = 0x12,
  JumpLiteral   = 0x13,
  JumpVar       = 0x14,
  SwitchLiteral = 0x15,
  SwitchVar     = 0x16,
  JumpIf        = 0x17,
};

} // namespace pl
