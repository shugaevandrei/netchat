#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

#include "dialogmodel.h"

#include <qqml.h>

class Client : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_INVOKABLE void newConnection();
    Q_INVOKABLE void postMessage(const QString &msg);
    Q_PROPERTY(dialogModel model READ messageModel);
    QML_ELEMENT

    explicit Client(QObject *parent = nullptr);


    //explicit Client(QObject *parent = nullptr);


    QString userName();
    //QAbstractListModel getModel(){return messageModel;};
    void setUserName(const QString &userName);

private slots:
    void onRedyRead();
    void onConnected();
    void onDisconnected();
signals:
    void userNameChanged();
private:
    QTcpSocket * mTcpSocket;
    dialogModel messageModel;
    QString m_userName = "BOBA";
};

#endif // CLIENT_H
