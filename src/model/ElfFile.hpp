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

  enum Type {
    Type_NULL          = 0,
    Type_PROGBITS      = 1,
    Type_SYMTAB        = 2,
    Type_STRTAB        = 3,
    Type_RELA          = 4,
    Type_HASH          = 5,
    Type_DYNAMIC       = 6,
    Type_NOTE          = 7,
    Type_NOBITS        = 8,
    Type_REL           = 9,
    Type_SHLIB         = 10,
    Type_DYNSYM        = 11,
    Type_INIT_ARRAY    = 14,
    Type_FINI_ARRAY    = 15,
    Type_PREINIT_ARRAY = 16,
    Type_GROUP         = 17,
    Type_SYMTAB_SHNDX  = 18,
  };

  enum Flags {
    Flags_WRITE            = (1 << 0),
    Flags_ALLOC            = (1 << 1),
    Flags_EXECINSTR        = (1 << 2),
    Flags_MERGE            = (1 << 3),
    Flags_STRINGS          = (1 << 4),
    Flags_INFO_LINK        = (1 << 5),
    Flags_LINK_ORDER       = (1 << 6),
    Flags_OS_NONCONFORMING = (1 << 7),
    Flags_GROUP            = (1 << 8),
    Flags_TLS              = (1 << 9),
  };
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
  uint8_t     visibility;

  enum Bind {
    Bind_LOCAL    = 0,
    Bind_GLOBAL   = 1,
    Bind_WEAK     = 2,
    Bind_LOOS     = 10,
    Bind_HIOS     = 12,
    Bind_MULTIDEF = 13,
    Bind_HIPROC   = 15,
  };

  enum Type {
    Type_NOTYPE            = 0,
    Type_OBJECT            = 1,
    Type_FUNC              = 2,
    Type_SECTION           = 3,
    Type_FILE              = 4,
    Type_COMMON            = 5,
    Type_TLS               = 6,
    Type_AMDGPU_HSA_KERNEL = 10,
    Type_HIOS              = 12,
    Type_LOPROC            = 13,
    Type_HIPROC            = 15,
  };

  enum Visibility {
    Visibility_DEFAULT   = 0,
    Visibility_INTERNAL  = 1,
    Visibility_HIDDEN    = 2,
    Visibility_PROTECTED = 3,
  };
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

  /** Checks if a file at @p path exists and starts with the ELF file header
   * @param path Path to a file on disk.
   * @return true if the file is an ELF file, false otherwise.
   */
  static bool isElfFile(const char* path);

  auto load(const char* file) -> bool;
  auto load(const std::string& file) -> bool;
  auto sections() const noexcept -> const std::vector<Section>&;
  auto symbols() const noexcept -> const std::vector<Symbol>&;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
