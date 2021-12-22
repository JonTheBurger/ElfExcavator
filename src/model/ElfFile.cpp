#include "ElfFile.hpp"

#include <elfio/elfio.hpp>
#include <fstream>
#include <llvm/Demangle/Demangle.h>
#include <spdlog/spdlog.h>
#include <vector>

struct ElfFile::Impl {
  ELFIO::elfio         elf_file;
  std::vector<Section> sections;
  std::vector<Symbol>  symbols;

  void loadSymbolsFrom(const ELFIO::symbol_section_accessor& symbol_reader)
  {
    const size_t symbol_count = symbol_reader.get_symbols_num();
    symbols.reserve(symbols.size() + symbol_count);

    for (size_t idx = 0; idx < symbol_count; ++idx)
    {
      symbols.emplace_back(idx);
      auto&& symbol    = symbols.back();
      bool   symbol_ok = symbol_reader.get_symbol(
        symbol.index,
        symbol.mangled_name,
        symbol.value,
        symbol.size,
        symbol.bind,
        symbol.type,
        symbol.section_index,
        symbol.visibility);
      symbol.name = llvm::demangle(symbol.mangled_name);

      //      if (symbol.name.empty())
      //      {
      //        spdlog::info("Dropping unnamed symbol");
      //        symbols.pop_back();
      //      }
      if (symbol_ok)
      {
        spdlog::info("Loaded Symbol: {} {} {:x} {:x}",
                     symbol.index,
                     symbol.name,
                     symbol.value,
                     symbol.size);
      }
      else
      {
        spdlog::info("Failed to load section");
        symbols.pop_back();
      }
    }
  }
};

static Section sectionFrom(ELFIO::section* section_header)
{
  Section section;
  section.index              = section_header->get_index();
  section.name               = section_header->get_name();
  section.type               = section_header->get_type();
  section.flags              = section_header->get_flags();
  section.info               = section_header->get_info();
  section.link               = section_header->get_link();
  section.addr_align         = section_header->get_addr_align();
  section.entry_size         = section_header->get_entry_size();
  section.address            = section_header->get_address();
  section.size               = section_header->get_size();
  section.name_string_offset = section_header->get_name_string_offset();
  section.offset             = section_header->get_offset();
  return section;
}

ElfFile::ElfFile()
    : _self{ std::make_unique<Impl>() }
{
}

ElfFile::~ElfFile() = default;

bool ElfFile::isElfFile(const char* path)
{
  static constexpr char MAGIC[] = { 0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  char                  buffer[sizeof(MAGIC)]{};
  std::ifstream         ifs(path, std::ios::binary);
  bool                  ok = false;

  if (ifs)
  {
    ifs.read(buffer, sizeof(buffer));
  }

  if (ifs.good())
  {
    ok = std::memcmp(MAGIC, buffer, sizeof(MAGIC)) == 0;
  }

  return ok;
}

bool ElfFile::load(const char* file)
{
  return load(std::string{ file });
}

bool ElfFile::load(const std::string& file)
{
  spdlog::info("[start] Loading Sections");
  bool ok = _self->elf_file.load(file);
  if (!ok) { spdlog::error("[end] Loading Sections"); }
  spdlog::info("[end] Loading Sections");

  _self->sections.reserve(_self->elf_file.sections.size());
  for (auto* section_header : _self->elf_file.sections)
  {
    _self->sections.push_back(sectionFrom(section_header));
    spdlog::info("Opened Section: {}", _self->sections.back());

    if ((section_header->get_type() != SHT_SYMTAB)) { continue; }
    const ELFIO::symbol_section_accessor symbol_reader(_self->elf_file, section_header);

    spdlog::info("[start] Loading Symbols");
    _self->loadSymbolsFrom(symbol_reader);
    spdlog::info("[end] Loading Symbols");
  }

  return ok;
}

const std::vector<Section>& ElfFile::sections() const noexcept
{
  return _self->sections;
}

const std::vector<Symbol>& ElfFile::symbols() const noexcept
{
  return _self->symbols;
}

std::string_view ElfFile::contentsOf(const Section& section) const
{
  auto idx = section.index;
  if (idx >= _self->elf_file.sections.size()) { return {}; }

  const auto* section_info = _self->elf_file.sections[idx];

  // If loading failed, we may need to seek for the exact index;
  if (section_info->get_index() != idx)
  {
    if (section_info->get_index() < idx)
    {
      // TODO: rfind
    }
  }

  return {
    _self->elf_file.sections[section.index]->get_data(),
    _self->elf_file.sections[section.index]->get_size()
  };
}

std::string ElfFile::disassemble(const Symbol& symbol) const
{
  return {};
}
