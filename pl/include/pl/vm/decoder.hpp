#pragma once

#include "../ir/opcode.hpp"
#include "arithmetic.hpp"
#include "executor.hpp"
#include "mmu.hpp"
#include "workspace.hpp"

namespace pl {

/**
 * TODO: finish decoding
 * TODO: finish commands family
 */

struct Decoder {
  static Command next(const Session &session) {}
};

} // namespace pl
