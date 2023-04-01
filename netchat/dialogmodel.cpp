#include "dialogmodel.h"

DialogModel::DialogModel(QObject *parent)
    :QAbstractListModel(parent)
{}

int DialogModel::rowCount(const QModelIndex& parent) const
{   
    if (parent.isValid()) {
        return 0;
    }

    return _messages.size();
}

QVariant DialogModel::data(const QModelIndex &index, int role) const
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

QHash<int, QByteArray> DialogModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ColorRole] = "color";
    roles[TextRole] = "text";

    return roles;
}

void DialogModel::add(const QString &msg)
{
    beginInsertRows(QModelIndex(), _messages.size(), _messages.size());
    _messages.append(msg);
    endInsertRows();

    //_messages[0] = QString("Size: %1").arg(_messages.size());
    //QModelIndex index = createIndex(0, 0, static_cast<void *>(0));
    //emit dataChanged(index, index);
}
