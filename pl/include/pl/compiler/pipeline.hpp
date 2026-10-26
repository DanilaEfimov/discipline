#pragma once

#include <algorithm>
#include <vector>

namespace plc {

enum class PlcPhase {
  TextPlugins,
  Tokenization,
  Preprocessing,
  Assembler,
};

class Pipeline {
  std::vector<PlcPhase> phases;

public:
  Pipeline(std::vector<PlcPhase> phases) : phases(phases) {};

  void skip(PlcPhase phase) {
    this->phases.erase(
        std::remove(this->phases.begin(), this->phases.end(), phase),
        this->phases.end());
  };

  void stop_at(std::vector<>)
};

} // namespace plc
