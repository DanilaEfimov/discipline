#pragma once

#include "decoder.hpp"
#include "excode.hpp"
#include "executor.hpp"
#include "workspace.hpp"
#include <istream>
#include <ostream>

namespace pl {

class Runtime {

  Workspace workspace;
  Executor executor;

  Command select_first_command() {
    return Decoder::next(this->workspace.get_current_session());
  }

public:
  Runtime(std::vector<Module> modules, std::ostream &out, std::istream &in)
      : workspace(modules, out, in), executor(workspace) {}

  ExCode run() {
    Command current = this->select_first_command();

    while (current.op != OpCode::End) {
      this->executor.run_command(current);
      current = Decoder::next(this->workspace.get_current_session());
    }

    return ExCode::Success;
  };
};

} // namespace pl
