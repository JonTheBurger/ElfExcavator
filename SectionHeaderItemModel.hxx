#ifndef SECTIONHEADERITEMMODEL_HXX
#define SECTIONHEADERITEMMODEL_HXX

#include <QtCore/QAbstractItemModel>
#include <vector>

#include "SectionHeader.hxx"

class SectionHeaderItemModel : public QAbstractItemModel {
  Q_OBJECT

public:
  enum Columns {
    NAME,
    SIZE,
    VMA,
    LMA,
    FILE_OFF,
    TYPES,
    ALIGN,

    MAX,
  };

  explicit SectionHeaderItemModel(std::vector<SectionHeader>& sectionHeaders, QObject* parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Basic functionality:
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  std::vector<SectionHeader>& _sectionHeaders;
};

#endif  // SECTIONHEADERITEMMODEL_HXX
