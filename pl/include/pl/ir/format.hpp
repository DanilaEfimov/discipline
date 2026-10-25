#pragma once

#include <cstddef>
#include <cstdint>

#define PL_MODULE_MAGIC 0xABBA2026
#ifndef PL_VERSION
#define PL_VERSION 1
#endif

namespace pl {

enum class SectionName { Header, Data };

using mlen_t = std::uint64_t; // module length size

template <SectionName S> struct SectionHeader {
  mlen_t length;
};

using sign_t = uint32_t;
using version_t = uint32_t;

inline constexpr std::size_t sign_offset = 0;
inline constexpr std::size_t version_offset = sizeof(sign_t) + sign_offset;

template <> struct SectionHeader<SectionName::Header> {
  static constexpr sign_t sign = PL_MODULE_MAGIC;
  static constexpr version_t version = PL_VERSION;
};

using ModuleHeader = SectionHeader<SectionName::Header>;
inline constexpr std::size_t module_header_size =
    sizeof(ModuleHeader::sign) + sizeof(ModuleHeader::version);

template <SectionName S> struct Section {
  SectionHeader<S> header;
  std::uint8_t *bytes = nullptr;
};

struct Module {
  Section<SectionName::Header> header;
  Section<SectionName::Data> data;
};

} // namespace pl
