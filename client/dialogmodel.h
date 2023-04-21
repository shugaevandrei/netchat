#pragma once

#include <QColor>
#include <QAbstractListModel>
#include <QObject>
#include <QDataStream>

struct Message {

    Message(QString t, QColor c);
    Message();

    QString text;
    QColor color;

    friend QDataStream& operator << (QDataStream &out, const Message& mess);
    friend QDataStream& operator >> (QDataStream &out, Message& mess);
};

class DialogModel: public QAbstractListModel
{
public:
    enum Roles {
        ColorRole = Qt::UserRole + 1,
        TextRole
    };
    DialogModel(QObject * parent = 0);

    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void add(const QString &msg, const QColor &color);
    void setModel(const QString &interlocutor);
    void saveCurrentModel();
    QHash<QString, QList<Message>> getModel();
    void setModel(QHash<QString, QList<Message>> model);
    void clearAll();

private:
    QHash<QString, QList<Message>> _allMessages;
    QList<Message> _messages;
    QString interlocutor_ ;
};

