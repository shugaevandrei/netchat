#pragma once

#include "dialogmodel.h"
#include "contactmodel.h"

#include <QObject>
#include <QTcpSocket>
#include <QQuickView>
#include <QDataStream>
#include <QJsonObject>
#include <QHash>
#include <QColor>
#include <QDataStream>
#include <QFile>

class Client : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY (bool isConnect READ isConnect NOTIFY isConnectChanged)
    Q_INVOKABLE void newConnection(const QString &ip, const quint16 &port);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void postMessage(const QString &msg, const QString &receiver, const QString &type = "sendMessage");
    Q_INVOKABLE void addContact(const QString &cont);
    Q_INVOKABLE void setCurReceiver(const QString &interlocutor);

    explicit Client(QObject *parent = nullptr);
    ~Client();

    bool isConnect();
    DialogModel& getModel();
    ContactModel& getContactModel();
    void saveDialogs();
    void readDialogs();

    quintptr getReceiver(const QString &name);

private slots:
    void onRedyRead();
    void onConnected();
    void onDisconnected();

signals:
    void isConnectChanged();

private:
    DialogModel messageModel;
    ContactModel contactModel;
    QTcpSocket * mTcpSocket;
    bool connect_ = false;
    QByteArray data;
    qint16 blockSize = 0;
    QHash<QString, qintptr> clients;
};
