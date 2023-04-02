#pragma once

#include <QColor>
#include <QAbstractListModel>
#include <QObject>

class DialogModel: public QAbstractListModel
{
public:
    enum Roles {
        ColorRole = Qt::UserRole + 1,
        TextRole
    };
    struct Message {
        Message(QString t, QColor c)
            : text(t), color(c){}

        QString text;
        QColor color;
    };
    DialogModel(QObject * parent = 0);
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void add(const QString &msg, const QColor &color);
    QList<Message> getModel(){return _messages;}
private:
    QList<Message> _messages;
};
