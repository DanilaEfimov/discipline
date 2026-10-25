#pragma once

#include "mmu.hpp"
#include "arithmetic.hpp"
#include "workspace.hpp"
#include "../ir/opcode.hpp"

namespace pl {

struct Command {
    OpCode op;
    value_t dst, a, b;
    ip_t imm;
};

struct Executor {

    MemoryManager& mmu;
    Arithmetic& calculator;
    Workspace& workspace;

    Executor(MemoryManager& mmu, Arithmetic& calculator, Workspace& workspace)
        : mmu(mmu), calculator(calculator), workspace(workspace) {}

    void run_command(Command& command) {

    }

};

} // namespace pl
