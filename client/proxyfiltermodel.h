#pragma once

#include <QSortFilterProxyModel>
#include "dialogmodel.h"

class ProxyFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum Filter{
        none,
        searchMessage
    };
    explicit ProxyFilterModel(QObject *parent = 0);
    void setFilterKind(Filter  filter);

private:
    Filter filter_ = none;
    QString key_;

public slots:
    void updateFilter(QString key);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};
