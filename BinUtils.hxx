#ifndef BINUTILS_HXX
#define BINUTILS_HXX

#include <QProcess>
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

  template <typename Function>
  void ExecObjdump(const QStringList& args, Function&& callback)
  {
    auto* process = new QProcess;
    QObject::connect(process,
                     static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     [this, process, callback{ std::move(callback) }](int exitCode, QProcess::ExitStatus exitStatus) {
                       if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
                       {
                         callback(process->readAllStandardOutput());
                       }
                       process->deleteLater();
                     });
    process->start(this->Objdump(), args);
  }

private:
  QString _objdump;
  QString _readelf;
  QString _nm;
  QString _elfFile;
};

#endif  // BINUTILS_HXX
