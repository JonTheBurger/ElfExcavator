#include "HexView.hpp"

#include <QPalette>
#include <QPushButton>
#include <document/buffer/qmemorybuffer.h>
#include <qhexview.h>

#include "model/ByteConversion.hpp"
#include "ui_HexView.h"

struct HexView::Impl {
  Q_DISABLE_COPY(Impl)

  HexView&    self;
  Ui::HexView ui;

  explicit Impl(HexView& that) noexcept
      : self{ that }
  {
    ui.setupUi(&self);
    connect(ui.next_button, &QPushButton::pressed, [this]() { onSearchNextClicked(); });
    connect(ui.previous_button, &QPushButton::pressed, [this]() { onSearchPreviousClicked(); });
    connect(ui.search_lineedit, &QLineEdit::returnPressed, [this]() { onSearchNextClicked(); });

    for (auto type = DataType::MIN_; type <= DataType::MAX_; type = static_cast<DataType>(static_cast<int>(type) + 1))
    {
      ui.type_combobox->addItem(nameof(type));
    }

    //    document->setHexLineWidth(8);
    //    QFont font = view->font();
    //    font.setPixelSize(font.pixelSize() + 2);
    //    view->setFont(font);
  }

  int selectedTypeSize(DataType type)
  {
    int size = 0;

    if (type == DataType::UTF8)
    {
      QByteArray::fromHex(ui.search_lineedit->text().toLatin1()).size();
    }
    else if (type == DataType::BYTES)
    {
      ui.search_lineedit->text().toUtf8().size();
    }
    else
    {
      size = sizeOf(type);
    }

    return size;
  }

  DataType selectedDataType() const noexcept
  {
    return static_cast<DataType>(ui.type_combobox->currentIndex());
  }

  void displaySelection()
  {
    const auto     idx   = ui.editor->document()->cursor()->selectionStart().offset();
    const DataType type  = selectedDataType();
    QByteArray     bytes = ui.editor->document()->read(idx, selectedTypeSize(type));
    QString        repr  = toString(std::move(bytes), type);

    ui.selection_lineedit->setText(repr);
  }

  void onSearchNextClicked()
  {
    const DataType   type  = selectedDataType();
    const QByteArray query = toBytes(ui.search_lineedit->text(), type);
    if (ui.editor->document()->searchForward(query) >= 0)
    {
      highlightCursor(query.size());
    }
  }

  void onSearchPreviousClicked()
  {
    const DataType   type  = selectedDataType();
    const QByteArray query = toBytes(ui.search_lineedit->text(), type);
    if (ui.editor->document()->searchBackward(query) >= 0)
    {
      highlightCursor(query.size());
    }
  }

  void highlightCursor(int length)
  {
    auto* document = ui.editor->document();
    auto* cursor   = document->cursor();
    document->metadata()->clear();
    document->metadata()->color(
      cursor->selectionStart().line,
      cursor->selectionStart().column,
      length,
      QPalette().highlightedText().color(),
      QPalette().highlight().color());
  }
};

HexView::HexView(QWidget* parent)
    : QWidget(parent)
    , _self{ std::make_unique<Impl>(*this) }
{
}

void HexView::setSource(QByteArray source)
{
  auto* document = QHexDocument::fromMemory<QMemoryBuffer>(source, this);
  _self->ui.editor->setDocument(document);
  connect(_self->ui.editor->document()->cursor(), &QHexCursor::positionChanged, [this]() { _self->displaySelection(); });
}

HexView::~HexView() = default;
