#pragma once

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHash>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
public slots:
    void slotNewConnection();
    void slotServerRead();
    void slotClientDisconnected();
   // void findReceiver(qintptr);

private:
    void sendMessageClient(QTcpSocket *sender, const QJsonObject jData);
    QTcpServer * mTcpServer;

    QSet<QTcpSocket *> mTcpSockets;
    QByteArray data;
    QHash<QString, qintptr> clients;
    QJsonObject json;
    quint16 blockSize;
};
