#pragma once

#include "protocol.h"

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
    bool _registration(QTcpSocket *sender, const QString &log, const QString &pass);
    bool _checkAccData(QTcpSocket *sender, const QString &log, const QString &pass);
    void _sendMessageClient(QTcpSocket *sender,
                            const PacketTypes::Types &type = PacketTypes::Types::ChatMessage,
                           const QString &msg = QString(),
                            qintptr receiverDesc = -1);
    QTcpSocket *_getReceiverSok(qintptr desc);
    QTcpServer *tcpServer_;
    QSet<QTcpSocket *> tcpSockets_;
    QByteArray data_;
    QHash<QString, qintptr> clients_;
    QHash<QString, QString> users_;
    quint16 blockSize_ = 0;
};
