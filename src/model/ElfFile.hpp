#pragma once

#include <memory>
#include <spdlog/fmt/fmt.h>
#include <string>
#include <vector>

struct Section {
  Section() = default;

  uint16_t    index;
  std::string name;
  uint32_t    type;
  uint64_t    flags;
  uint32_t    info;
  uint32_t    link;
  uint64_t    addr_align;
  uint64_t    entry_size;
  uint64_t    address;
  uint64_t    size;
  uint32_t    name_string_offset;
  uint64_t    offset;
};

template <>
struct fmt::formatter<Section> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); }

  template <typename FormatContext>
  auto format(const Section& self, FormatContext& ctx) -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "{} {} {:x} {:x}", self.index, self.name, self.address, self.size);
  }
};

struct Symbol {
  Symbol() = default;

  explicit Symbol(size_t symbol_index)
      : index{ symbol_index }
  {
  }

  uint64_t    index;
  std::string name;
  std::string mangled_name;
  uint64_t    value;
  uint64_t    size;
  uint8_t     bind;
  uint8_t     type;
  uint16_t    section_index;
  uint8_t     other;
};

template <>
struct fmt::formatter<Symbol> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); }

  template <typename FormatContext>
  auto format(const Symbol& self, FormatContext& ctx) -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "{} {} {:x} {:x}", self.index, self.name, self.value, self.size);
  }
};

class ElfFile final {
public:
  ElfFile();
  ~ElfFile();
  ElfFile(const ElfFile&) = delete;
  ElfFile& operator=(const ElfFile&) = delete;

  auto load(const std::string& file) -> bool;
  auto sections() const noexcept -> const std::vector<Section>&;
  auto symbols() const noexcept -> const std::vector<Symbol>&;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
