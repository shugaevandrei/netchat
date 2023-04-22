#pragma once

#include <QSortFilterProxyModel>
#include "dialogmodel.h"

namespace FilterTypes {
  Q_NAMESPACE
  enum Filter {
      NoneFiler,
      SearchMessage,
      TEST
  };
  Q_ENUMS(Filter)
}

class ProxyFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:

    explicit ProxyFilterModel(QObject *parent = 0);
    void setFilterKind(FilterTypes::Filter  filter);

private:
    FilterTypes::Filter filter_ = FilterTypes::NoneFiler;
    QString key_;

public slots:
    void updateFilter(QString key);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

