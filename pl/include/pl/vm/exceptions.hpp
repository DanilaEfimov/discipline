#pragma once

#include <stdexcept>
#include <string>

#define PL_BASE_EXCEPTION PlException
#define PL_EXCEPTION(name)                                                     \
  struct name : public PL_BASE_EXCEPTION {                                     \
    name(const std::string &what) : PL_BASE_EXCEPTION(what) {}                 \
  }

namespace pl {

struct PL_BASE_EXCEPTION : public std::runtime_error {
  explicit PL_BASE_EXCEPTION(const std::string &what)
      : std::runtime_error(what) {}
};

PL_EXCEPTION(PlUndefinedCommand);

PL_EXCEPTION(PlSegmentFault);

PL_EXCEPTION(PlRuntimeError);

PL_EXCEPTION(PlDivisionByZero);

} // namespace pl
