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
    struct User
    {
        User(QString log, bool st)
            :login(log), status(st){};
        QString login;
        bool status;
    };

    ContactModel(QObject * parent = 0);

    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void add(const User &msg);
    //QStringList getModel(){return _contact;}
    void clearAll();

private:
    //QStringList _contact;
    QList<User> _contact;
};
