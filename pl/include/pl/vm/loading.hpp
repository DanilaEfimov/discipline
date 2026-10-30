#pragma once

#include "../ir/format.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace pl {

inline Module load_module(fs::path path) {
  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error(std::string("no such module: ") +
                             std::string(path));

  std::uint8_t buf[module_header_size];
  file.read(reinterpret_cast<char *>(buf), sizeof(buf));
  if (file.gcount() != std::streamsize(sizeof(buf)))
    throw std::runtime_error(std::string("too short module: ") +
                             std::string(path));

  sign_t sign;
  version_t version;
  memcpy(&sign, buf, sizeof(sign_t));
  memcpy(&version, buf + version_offset, sizeof(version_t));

  if (sign != PL_MODULE_MAGIC)
    throw std::runtime_error(
        std::string("module's signature does not match: ") + std::string(path) +
        ": " + std::to_string(sign));
  if (version < PL_VERSION)
    throw std::runtime_error(
        std::string("module's version is not supported: ") +
        std::to_string(version));

  Module module;

  mlen_t length = 0;
  file.read(reinterpret_cast<char *>(&length), std::streamsize(sizeof(length)));
  if (!file)
    throw std::runtime_error(std::string("too short module: ") +
                             std::string(path));

  std::uint8_t *bytes = new std::uint8_t[length];
  file.read(reinterpret_cast<char *>(bytes), length);
  if (!file)
    throw std::runtime_error(std::string("too short module: ") +
                             std::string(path));

  module.data.bytes = bytes;
  module.data.header.length = length;

  return module;
}

inline std::vector<Module> load_modules(std::vector<std::string> paths) {
  std::vector<Module> modules(paths.size());
  for (std::size_t i = 0; i < paths.size(); ++i) {
    modules[i] = load_module(fs::path(paths[i]));
  }
  return modules;
}

} // namespace pl
