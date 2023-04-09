#pragma once

#include <QAbstractListModel>
#include <QObject>

class ContactModel: public QAbstractListModel
{
public:
    enum Roles {
        ColorRole = Qt::UserRole + 1,
        TextRole
    };
    ContactModel(QObject * parent = 0);
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void add(const QString &msg);
    QStringList getModel(){return _contact;}
    void clearAll();
private:
    QStringList _contact;
};
