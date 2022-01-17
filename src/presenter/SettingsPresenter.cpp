#include "SettingsPresenter.hpp"

#include <QStandardPaths>
#include <QString>

#include "model/ElfFile.hpp"

struct SettingsPresenter::Impl {
  Q_DISABLE_COPY(Impl)

  SettingsPresenter& self;
  ElfFile&           elf_file;
  QString            elf_file_path;
  QString            objdump_path;

  explicit Impl(SettingsPresenter& that, ElfFile& elf_file_) noexcept
      : self{ that }
      , elf_file{ elf_file_ }
      , elf_file_path{}
      , objdump_path{ QStandardPaths::findExecutable("objdump") }
  {
  }
};

SettingsPresenter::SettingsPresenter(ElfFile& elf_file, QObject* parent)
    : QObject(parent)
    , _self{ std::make_unique<Impl>(*this, elf_file) }
{
}

const QString& SettingsPresenter::elfFile() const noexcept
{
  return _self->elf_file_path;
}

void SettingsPresenter::setElfFile(const QString& elf_file)
{
  if (_self->elf_file_path != elf_file)
  {
    _self->elf_file_path = elf_file;

    bool is_valid = _self->elf_file.load(_self->elf_file_path.toStdString());

    emit elfFileChanged(_self->elf_file_path, is_valid);
  }
}

const QString& SettingsPresenter::objdumpPath() const noexcept
{
  return _self->objdump_path;
}

void SettingsPresenter::setObjdumpPath(const QString& objdump_path)
{
  if (_self->objdump_path != objdump_path)
  {
    _self->objdump_path = objdump_path;
    emit objdumpPathChanged(_self->objdump_path);
  }
}

SettingsPresenter::~SettingsPresenter() = default;
