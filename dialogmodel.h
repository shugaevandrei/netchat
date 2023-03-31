#pragma once

#include <QAbstractListModel>
#include <QObject>

class dialogModel: public QAbstractListModel
{
public:
    enum Roles {
        ColorRole = Qt::UserRole + 1,
        TextRole
    };
    dialogModel(QObject * parent = 0);
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void add();
    QStringList getModel(){return _messages;}
private:
    QStringList _messages;
};
