#include "proxyfiltermodel.h"

ProxyFilterModel::ProxyFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{}

void ProxyFilterModel::setFilterKind(Filter filter)
{
    filter_ = filter;
    invalidateFilter();
}

void ProxyFilterModel::updateFilter(QString key)
{
    key_ = key;
    invalidateFilter();
}

bool ProxyFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    QVariant value = sourceModel()->data(index, DialogModel::Roles::TextRole);
    if(value.isValid()){
        if (filter_ == none)
            return true;
        else  if (filter_ == searchMessage) {
            QString val = value.toString();
            return value.toString().contains(key_);
        }
    }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
