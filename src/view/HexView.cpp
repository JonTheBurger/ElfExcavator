#include "HexView.hpp"

#include <QPushButton>
#include <document/buffer/qmemorybuffer.h>
#include <qhexview.h>

#include "ui_HexView.h"

enum class DataType {
  U8,
  I8,
  U16,
  I16,
  U32,
  I32,
  U64,
  I64,
  FLOAT,
  DOUBLE,
  BYTES,
  UTF8,

  ARRAY_SIZE_,
  MAX_ = ARRAY_SIZE_ - 1,
  MIN_ = U8,
};

static constexpr bool isNumber(DataType type)
{
  return ((DataType::U8 <= type) && (type <= DataType::DOUBLE));
}

// TODO: should use input size
static constexpr int typeSize(DataType type)
{
  switch (type)
  {
    case DataType::U8:
      return 1;
    case DataType::I8:
      return 1;
    case DataType::U16:
      return 2;
    case DataType::I16:
      return 2;
    case DataType::U32:
      return 4;
    case DataType::I32:
      return 4;
    case DataType::U64:
      return 8;
    case DataType::I64:
      return 8;
    case DataType::FLOAT:
      return 4;
    case DataType::DOUBLE:
      return 8;
    default:
      break;
  }
  return 0;
}

static QString nameof(DataType type)
{
  switch (type)
  {
    case DataType::U8:
      return QStringLiteral("uint8_t");
    case DataType::I8:
      return QStringLiteral("uint8_t");
    case DataType::U16:
      return QStringLiteral("uint16_t");
    case DataType::I16:
      return QStringLiteral("int16_t");
    case DataType::U32:
      return QStringLiteral("uint32_t");
    case DataType::I32:
      return QStringLiteral("int32_t");
    case DataType::U64:
      return QStringLiteral("uint64_t");
    case DataType::I64:
      return QStringLiteral("int64_t");
    case DataType::FLOAT:
      return QStringLiteral("float");
    case DataType::DOUBLE:
      return QStringLiteral("double");
    case DataType::BYTES:
      return QStringLiteral("bytes");
    case DataType::UTF8:
      return QStringLiteral("utf-8");
    default:
      break;
  }
  return "";
}

template <typename T>
static QString toString(QByteArray bytes)
{
  // TODO: If Host Endian != Target Endian
  if (BYTE_ORDER == BIG_ENDIAN)
  {
    std::reverse(bytes.begin(), bytes.end());
  }

  if (static_cast<size_t>(bytes.size()) < sizeof(T))
  {
    return "";
  }

  T number;
  static_assert(std::is_trivially_copyable_v<T>);
  memcpy(&number, bytes.data(), sizeof(number));

  if constexpr (std::is_floating_point_v<T>)
  {
    return QString::number(static_cast<double>(number));
  }
  else
  {
    return QString::number(number, 16);
  }
}

static QString toString(QByteArray bytes, DataType type)
{
  if (type == DataType::BYTES)
  {
    return bytes.toHex();
  }
  else if (type == DataType::UTF8)
  {
    return QString::fromUtf8(bytes.data(), bytes.size());
  }

  switch (type)
  {
    case DataType::U8:
      return toString<uint8_t>(std::move(bytes));
    case DataType::I8:
      return toString<int8_t>(std::move(bytes));
    case DataType::U16:
      return toString<uint16_t>(std::move(bytes));
    case DataType::I16:
      return toString<int16_t>(std::move(bytes));
    case DataType::U32:
      return toString<uint32_t>(std::move(bytes));
    case DataType::I32:
      return toString<int32_t>(std::move(bytes));
    case DataType::U64:
      return toString<uint64_t>(std::move(bytes));
    case DataType::I64:
      return toString<int64_t>(std::move(bytes));
    case DataType::FLOAT:
      return toString<float>(std::move(bytes));
    case DataType::DOUBLE:
      return toString<double>(std::move(bytes));
    default:
      break;
  }
  return "";
}

template <typename T>
static QByteArray toBytes(const QString& repr)
{
  QByteArray bytes;
  if constexpr (std::is_floating_point_v<T>)
  {
    double x = repr.toDouble();
    bytes.append(reinterpret_cast<char*>(&x), sizeof(x));
  }
  else if constexpr (std::is_signed_v<T>)
  {
    T x = static_cast<T>(repr.toLongLong(nullptr, 16));
    bytes.append(reinterpret_cast<char*>(&x), sizeof(x));
  }
  else
  {
    T x = static_cast<T>(repr.toULongLong(nullptr, 16));
    bytes.append(reinterpret_cast<char*>(&x), sizeof(x));
  }

  // TODO: If Host Endian != Target Endian
  if (BYTE_ORDER == BIG_ENDIAN)
  {
    std::reverse(bytes.begin(), bytes.end());
  }
  return bytes;
}

QByteArray toBytes(QString repr, DataType type)
{
  if (type == DataType::BYTES)
  {
    QByteArray hex;
    hex += repr;
    return QByteArray::fromHex(hex);
  }
  else if (type == DataType::UTF8)
  {
    return repr.toUtf8();
  }

  switch (type)
  {
    case DataType::U8:
      return toBytes<uint8_t>(repr);
    case DataType::I8:
      return toBytes<int8_t>(repr);
    case DataType::U16:
      return toBytes<uint16_t>(repr);
    case DataType::I16:
      return toBytes<int16_t>(repr);
    case DataType::U32:
      return toBytes<uint32_t>(repr);
    case DataType::I32:
      return toBytes<int32_t>(repr);
    case DataType::U64:
      return toBytes<uint64_t>(repr);
    case DataType::I64:
      return toBytes<int64_t>(repr);
    case DataType::FLOAT:
      return toBytes<float>(repr);
    case DataType::DOUBLE:
      return toBytes<double>(repr);
    default:
      break;
  }

  return {};
}

struct HexView::Impl {
  Q_DISABLE_COPY(Impl)

  HexView&    self;
  Ui::HexView ui;

  explicit Impl(HexView& that) noexcept
      : self{ that }
  {
    ui.setupUi(&self);
    connect(ui.next_button, &QPushButton::pressed, [this](){ onSearchNextClicked(); });
    connect(ui.previous_button, &QPushButton::pressed, [this](){ onSearchPreviousClicked(); });

    for (auto type = DataType::MIN_; type <= DataType::MAX_; type = static_cast<DataType>(static_cast<int>(type) + 1))
    {
      ui.type_combobox->addItem(nameof(type));
    }

    // TODO: Implement Search
    //    QHexMetadata* data     = document->metadata();
    //  data->comment(1, 1, 2, "lol");
    //      document->setHexLineWidth(8);
    //    QHexCursor* cursor = document->cursor();
    //    cursor->moveTo(32);
    //        QFont font = view->font();
    //    font.setPixelSize(font.pixelSize() + 2);
    //    view->setFont(font);
    //    self.setSource(asdf, sizeof(asdf));
  }

  DataType selectedDataType() const noexcept
  {
    return static_cast<DataType>(ui.type_combobox->currentIndex());
  }

  void displaySelection()
  {
    const auto idx = ui.editor->document()->cursor()->selectionStart().offset();
    const DataType type = selectedDataType();
    QByteArray bytes = ui.editor->document()->read(idx, typeSize(type));
    QString repr = toString(std::move(bytes), type);

    ui.selection_lineedit->setText(repr);
    if (isNumber(type))
    {
      ui.selection_lineedit->setToolTip(QString::number(repr.toInt(nullptr, 16)));
    }
    else
    {
      ui.selection_lineedit->setToolTip("");
    }
  }

  void onSearchNextClicked()
  {
    const DataType type = selectedDataType();
    const QByteArray query = toBytes(ui.search_lineedit->text(), type);
    auto* document = ui.editor->document();
    auto idx = document->searchForward(query);
    if (idx < 0)
    {
      return;
    }
    auto* cursor = document->cursor();
    document->metadata()->clear();
    document->metadata()->color(cursor->position().line, cursor->position().column, typeSize(type), QColor(0xFF0000), QColor(0xffffff));
  }

  void onSearchPreviousClicked()
  {
    const DataType type = selectedDataType();
    const QByteArray query = toBytes(ui.search_lineedit->text(), type);
    auto* document = ui.editor->document();
    auto idx = document->searchBackward(query);
    if (idx < 0)
    {
      return;
    }
    auto* cursor = document->cursor();
    document->metadata()->clear();
    document->metadata()->color(cursor->position().line, cursor->position().column, typeSize(type), QColor(0xFF0000), QColor(0xffffff));
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
  connect(_self->ui.editor->document()->cursor(), &QHexCursor::positionChanged, [this](){ _self->displaySelection(); });
}

HexView::~HexView() = default;
