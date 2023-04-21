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
        return _messages.at(index.row()).color;
    case TextRole:
        return _messages.at(index.row()).text;
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

void DialogModel::add(const QString &msg, const QColor &color)
{
    beginInsertRows(QModelIndex(), _messages.size(), _messages.size());
    _messages.append(Message(msg, color));
    endInsertRows();

    //_messages[0] = QString("Size: %1").arg(_messages.size());
    //QModelIndex index = createIndex(0, 0, static_cast<void *>(0));
    //emit dataChanged(index, index);
}

void DialogModel::setModel(const QString &interlocutor)
{
    saveCurrentModel();
    clearAll();
    interlocutor_ = interlocutor;

    for(int i = 0; i < _allMessages.value(interlocutor_).length(); i++) {
        beginInsertRows(QModelIndex(), i, i);
        _messages.append(_allMessages.value(interlocutor_)[i]);
        endInsertRows();
    }
}

void DialogModel::saveCurrentModel()
{
    if(!_messages.isEmpty())
        _allMessages.insert(interlocutor_, _messages);
}

QHash<QString, QList<Message> > DialogModel::getModel()
{
    return _allMessages;
}

void DialogModel::setModel(QHash<QString, QList<Message> > model)
{
    _allMessages = model;
}

void DialogModel::clearAll()
{
    beginResetModel();
    _messages.clear();
    endResetModel();
}

Message::Message(QString t, QColor c)
{
    text = t;
    color= c;
}

Message::Message()
{}

QDataStream& operator << (QDataStream &out, const Message& mess)
{
    out << mess.text;
    out << mess.color;
    return out;
};

QDataStream& operator >> (QDataStream &out, Message& mess)
{
    out >> mess.text;
    out >> mess.color;
    return out;
};
