#pragma once

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHash>
#include <QPair>

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
    bool registration (const QString &log, const QString &pass);
    bool checkAccData(QTcpSocket *sender, const QString &log, const QString &pass);
    void sendMessageClient(QTcpSocket *sender, const QJsonObject jData, qintptr receiverDesc = -1);
    QTcpSocket* getReceiverSok(qintptr desc);
    QTcpServer * mTcpServer;
    QSet<QTcpSocket *> mTcpSockets;
    QByteArray data;
    QHash<QString, qintptr> clients;
    QHash<QString, QString> users;
    QJsonObject json;
    quint16 blockSize = 0;
};
