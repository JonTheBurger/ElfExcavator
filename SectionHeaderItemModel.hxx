#ifndef SECTIONHEADERITEMMODEL_HXX
#define SECTIONHEADERITEMMODEL_HXX

#include <QAbstractTableModel>
#include <vector>

#include "SectionHeader.hxx"

class SectionHeaderItemModel : public QAbstractTableModel {
  Q_OBJECT

public:
  enum Columns {
    INDEX,
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
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  // Edit
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  bool          setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

private:
  std::vector<SectionHeader>& _sectionHeaders;
};

#endif  // SECTIONHEADERITEMMODEL_HXX
