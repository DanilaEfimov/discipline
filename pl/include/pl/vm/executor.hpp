#pragma once

#include "../ir/opcode.hpp"
#include "arithmetic.hpp"
#include "mmu.hpp"
#include "workspace.hpp"

namespace pl {

using command_t = value_t;

struct Command {
  OpCode op;
  value_t dst, a, b;
  ip_t imm;
};

struct Executor {

  MemoryManager &mmu;
  Arithmetic &calculator;
  Workspace &workspace;

  Executor(MemoryManager &mmu, Arithmetic &calculator, Workspace &workspace)
      : mmu(mmu), calculator(calculator), workspace(workspace) {}

  void run_command(Command &command) {}
};

} // namespace pl
