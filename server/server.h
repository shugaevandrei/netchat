#pragma once

#include "protocol.h"

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHash>
#include <QSqlDatabase>

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
                            QString receiver = "-1"
                            /*qintptr receiverDesc*/ );
    void _addNewContact(QTcpSocket *sender ,const QString &login);
   // QTcpSocket *_getReceiverSok(qintptr desc);
    QTcpSocket *_getReceiverSok(const QString &login);
    QTcpServer *tcpServer_;
    QHash<QTcpSocket *, QString> tcpSockets_;
    QByteArray data_;
   // QHash<QString, qintptr> clients_;//возможно не нужно
    //QHash<QString, QString> users_;
    quint16 blockSize_ = 0;
    QSqlDatabase db_;
};
