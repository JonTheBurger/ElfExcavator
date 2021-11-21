#pragma once

#include <QObject>
#include <memory>

class QString;
class ElfFile;

class SettingsPresenter final : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(SettingsPresenter)
  Q_PROPERTY(QString elf_file READ elfFile WRITE setElfFile NOTIFY elfFileChanged FINAL)
  Q_PROPERTY(QString objdump_path READ objdumpPath WRITE setObjdumpPath NOTIFY objdumpPathChanged FINAL)

public:
  SettingsPresenter(ElfFile& elf_file, QObject* parent = nullptr);
  ~SettingsPresenter() override;

  const QString& elfFile() const noexcept;
  void           setElfFile(const QString& elf_file);

  const QString& objdumpPath() const noexcept;
  void           setObjdumpPath(const QString& objdump_path);

signals:
  void elfFileChanged(const QString& elf_file, bool is_valid);
  void objdumpPathChanged(const QString& objdump_path);

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
