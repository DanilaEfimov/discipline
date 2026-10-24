#pragma once

#include <cstdint>

namespace pl {

enum OpCode : uint8_t {
    AddLiteral   = 0x01,
    AddVar       = 0x02,
    MulLiteral   = 0x03,
    MulVar       = 0x04,
    DivLiteral   = 0x05,
    DivVar       = 0x06,
    RightLiteral = 0x07,
    RightVar     = 0x08,
    LeftLiteral  = 0x09,
    LeftVar      = 0x0A,
    WriteLiteral = 0x0B,
    WriteVar     = 0x0C,
    Print        = 0x0D,
    Read         = 0x0E,
    JumpLiteral  = 0x0F,
    JumpVar      = 0x10,
    End          = 0x11,
};

} // namespace pl
