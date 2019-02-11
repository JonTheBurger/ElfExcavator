#include "BinUtils.hxx"

#include <QSettings>

BinUtils::BinUtils()
{
  QSettings settings;
  SetObjdump(settings.value("objdump", "").toString());
  SetReadelf(settings.value("readelf", "").toString());
  SetNm(settings.value("nm", "").toString());
  SetElfFile(settings.value("elfFile", "").toString());
}

BinUtils::~BinUtils()
{
  QSettings settings;
  settings.setValue("objdump", Objdump());
  settings.setValue("readelf", Readelf());
  settings.setValue("nm", Nm());
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

const QString& BinUtils::Readelf() const
{
  return _readelf;
}

bool BinUtils::SetReadelf(const QString& value)
{
  // TODO: Check valid readelf (empty is not valid)
  _readelf = value;
  return true;
}

const QString& BinUtils::Nm() const
{
  return _nm;
}

bool BinUtils::SetNm(const QString& value)
{
  // TODO: Check valid nm (empty is not valid)
  _nm = value;
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
