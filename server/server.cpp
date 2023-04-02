#include "server.h"
#include <QDebug>
#include <QCoreApplication>

Server::Server(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection, this, &Server::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 6000)){
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started";
    }
    blockSize = 0;

    clients.insert("127.0.0.2",3);
    clients.insert("127.0.0.3",2);
    clients.insert("127.0.0.4",3);
}

void Server::slotNewConnection()
{
    QTcpSocket  *mTcpSocket = mTcpServer->nextPendingConnection();
    QJsonObject preambMessage;

    if(!mTcpSockets.contains(mTcpSocket)){

        preambMessage.insert("message", "msg");
        preambMessage.insert("type", "send_clients");
        sendMessageClient(mTcpSocket, preambMessage);

        connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
        connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);
        mTcpSockets.insert(mTcpSocket);
        clients.insert(mTcpSocket->peerName(), mTcpSocket->peerPort());
    }
}

void Server::slotServerRead()
{
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream in(sender);
    in.setVersion(QDataStream::Qt_6_4);

    if(in.status() ==QDataStream::Ok){
        while(1){
            if(blockSize == 0){
                if(sender->bytesAvailable() < 2)
                    break;
                in >> blockSize;
            }
            if(sender->bytesAvailable() < blockSize)
                break;

            QJsonObject str;
            QJsonValue val;

            in >> str;
            blockSize = 0;
            sendMessageClient(sender, str);
            break;
        }
    }
}

void Server::slotClientDisconnected()
{
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());
    sender->close();
    sender->deleteLater();//?
    mTcpSockets.remove(sender);
}

void Server::sendMessageClient(QTcpSocket *sender, const QJsonObject jData)
{
    data.clear();

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    if(jData.value("type").toString() == "send_clients")
        out << quint16(0) << jData << clients;
    else
        out << quint16(0) << jData;
    out.device()->seek(0);
    out <<quint16(data.size() - sizeof(quint16));
    sender->write(data);
}
