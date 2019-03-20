#include "BinUtils.hxx"

#include <QSettings>

BinUtils::BinUtils()
{
  QSettings settings;
  SetObjdump(settings.value("objdump", "").toString());
  SetElfFile(settings.value("elfFile", "").toString());
}

BinUtils::~BinUtils()
{
  QSettings settings;
  settings.setValue("objdump", Objdump());
  settings.setValue("elfFile", ElfFile());
}

const QString& BinUtils::Objdump() const
{
  return _objdump;
}

bool BinUtils::SetObjdump(const QString& value)
{
  // TODO: Check valid objdump (empty is not valid)
  _objdump = value;
  return true;
}

const QString& BinUtils::ElfFile() const
{
  return _elfFile;
}

bool BinUtils::SetElfFile(const QString& value)
{
  // TODO: Check valid ELF (empty is not valid)
  _elfFile = value;
  return true;
}
