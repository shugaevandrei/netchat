#include "contactmodel.h"

ContactModel::ContactModel(QObject *parent)
    :QAbstractListModel(parent)
{}

int ContactModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return _contact.size();
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case ColorRole:
        return _contact.at(index.row()).status ? "lightseagreen" : "dimgray";
    case TextRole:
        return _contact.at(index.row()).login;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ContactModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ColorRole] = "color";
    roles[TextRole] = "text";

    return roles;
}

void ContactModel::add(const User &msg)
{
    beginInsertRows(QModelIndex(), _contact.size(), _contact.size());
    _contact.append(msg);
    endInsertRows();
}

void ContactModel::clearAll()
{
    beginResetModel();
    _contact.clear();
    endResetModel();
}
