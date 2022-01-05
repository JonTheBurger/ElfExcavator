#pragma once

#include <QWidget>
#include <memory>

class HexView final : public QWidget {
  Q_OBJECT
  Q_DISABLE_COPY(HexView)

public:
  explicit HexView(QWidget* parent = nullptr);
  ~HexView() override;

  void setSource(QByteArray source);

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
