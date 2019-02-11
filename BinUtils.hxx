#ifndef BINUTILS_HXX
#define BINUTILS_HXX

#include <QString>

class BinUtils {
  Q_DISABLE_COPY(BinUtils)

public:
  BinUtils();
  ~BinUtils();

  const QString& Objdump() const;
  bool           SetObjdump(const QString& value);
  const QString& Readelf() const;
  bool           SetReadelf(const QString& value);
  const QString& Nm() const;
  bool           SetNm(const QString& value);
  const QString& ElfFile() const;
  bool           SetElfFile(const QString& value);

private:
  QString _objdump;
  QString _readelf;
  QString _nm;
  QString _elfFile;
};

#endif  // BINUTILS_HXX
