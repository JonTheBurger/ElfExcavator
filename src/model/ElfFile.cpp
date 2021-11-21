#include "ElfFile.hpp"

#include <elfio/elfio.hpp>
#include <llvm/Demangle/Demangle.h>
#include <spdlog/spdlog.h>
#include <vector>

struct ElfFile::Impl {
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
        symbol.other);
      symbol.name = llvm::demangle(symbol.mangled_name);

      if (symbol.name.empty())
      {
        spdlog::info("Dropping unnamed symbol");
        symbols.pop_back();
      }
      else if (symbol_ok)
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

bool ElfFile::load(const std::string& file)
{
  spdlog::info("[start] Loading Sections");
  ELFIO::elfio elf_file;
  bool         ok = elf_file.load(file);
  if (!ok) { spdlog::error("[end] Loading Sections"); }
  spdlog::info("[end] Loading Sections");

  _self->sections.reserve(elf_file.sections.size());
  for (auto* section_header : elf_file.sections)
  {
    _self->sections.push_back(sectionFrom(section_header));
    spdlog::info("Opened Section: {}", _self->sections.back());

    if ((section_header->get_type() != SHT_SYMTAB)) { continue; }
    const ELFIO::symbol_section_accessor symbol_reader(elf_file, section_header);

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
