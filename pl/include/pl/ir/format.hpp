#pragma once

#include <cstddef>
#include <cstdint>

#define PL_MODULE_MAGIC 0xABBA2026
#ifndef PL_VERSION
#define PL_VERSION 1
#endif

namespace pl {

enum class SectionName { Header, Data };

template <SectionName S> struct SectionHeader {
  std::size_t length;
};

template <> struct SectionHeader<SectionName::Header> {
  static constexpr uint32_t sign = PL_MODULE_MAGIC;
  static constexpr uint32_t version = PL_VERSION;
};

inline constexpr std::size_t module_header_size =
    sizeof(SectionHeader<SectionName::Header>);

template <SectionName S> struct Section {
  SectionHeader<S> header;
  std::uint8_t *bytes = nullptr;
};

struct Module {
  Section<SectionName::Header> header;
  Section<SectionName::Data> data;
};

} // namespace pl
