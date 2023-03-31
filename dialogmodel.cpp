#include "dialogmodel.h"

dialogModel::dialogModel(QObject *parent)
    :QAbstractListModel(parent)
{
    add();
    add();
}

int dialogModel::rowCount(const QModelIndex& parent) const
{   
    if (parent.isValid()) {
        return 0;
    }

    return _messages.size();
}

QVariant dialogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
        case ColorRole:
            return QVariant(index.row() < 2 ? "orange" : "skyblue");
        case TextRole:
            return _messages.at(index.row());
        default:
            return QVariant();
  }
}

QHash<int, QByteArray> dialogModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ColorRole] = "color";
    roles[TextRole] = "texts";

    return roles;
}

void dialogModel::add()
{
    beginInsertRows(QModelIndex(), _messages.size(), _messages.size());
    _messages.append("new");
    endInsertRows();

    //_messages[0] = QString("Size: %1").arg(_messages.size());
    //QModelIndex index = createIndex(0, 0, static_cast<void *>(0));
    //emit dataChanged(index, index);
}
