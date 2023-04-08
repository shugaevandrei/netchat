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

private:
    void sendMessageClient(QTcpSocket *sender, const QJsonObject jData, qintptr receiverDesc = -1);
    QTcpSocket* getReceiverSok(qintptr desc);
    QTcpServer * mTcpServer;
    QSet<QTcpSocket *> mTcpSockets;
    QByteArray data;
    QHash<QString, qintptr> clients;
    QJsonObject json;
    quint16 blockSize = 0;
};
