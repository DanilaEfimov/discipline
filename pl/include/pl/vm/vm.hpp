#pragma once

#include "exceptions.hpp"
#include "excode.hpp"
#include "runtime.hpp"
#include <iostream>
#include <string>

namespace pl {

struct RunResult {
  std::string error;
  ExCode code;

  RunResult(ExCode code, std::string error) : code(code), error(error) {}
};

class VM {
  Runtime runtime;

public:
  VM(std::vector<Module> modules, std::ostream &out, std::istream &in)
      : runtime(std::move(modules), out, in) {}

  RunResult run() {
    std::string error_string;
    ExCode code = ExCode::Success;

    try {
      code = this->runtime.run();
    } catch (PlSegmentFault &e) {
      error_string = e.what();
      code = ExCode::SegmentFault;
    } catch (PlUndefinedCommand &e) {
      error_string = e.what();
      code = ExCode::UndefinedCommand;
    } catch (std::exception &e) {
      error_string = e.what();
      code = ExCode::RuntimeError;
    }

    return RunResult{code, error_string};
  }
};

} // namespace pl
