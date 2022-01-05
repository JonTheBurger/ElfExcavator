#include "HexView.hpp"

#include <document/buffer/qmemorybuffer.h>
#include <qhexview.h>

#include "ui_HexView.h"

struct HexView::Impl {
  Q_DISABLE_COPY(Impl)

  HexView&    self;
  Ui::HexView ui;

  explicit Impl(HexView& that) noexcept
      : self{ that }
  {
    ui.setupUi(&self);
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
}

HexView::~HexView() = default;
