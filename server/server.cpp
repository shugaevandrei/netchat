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

    clients.insert("2222",2323);
    clients.insert("5455",275323);
    clients.insert("6532",78);
}

void Server::slotNewConnection()
{
    QTcpSocket  *mTcpSocket = mTcpServer->nextPendingConnection();
    QJsonObject preambMessage;

    if(!mTcpSockets.contains(mTcpSocket)){
        connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
        connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);




        mTcpSockets.insert(mTcpSocket);
        clients.insert(mTcpSocket->peerAddress().toString(), mTcpSocket->socketDescriptor());

         preambMessage.insert("type", "send_clients");
        for (auto sok: mTcpSockets){
            sendMessageClient(sok, preambMessage);
        }
    }
}

void Server::slotServerRead()
{
    qDebug()<<"slotServerRead";
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream in(sender);
    in.setVersion(QDataStream::Qt_6_4);
    if(in.status() ==QDataStream::Ok){
        for(;;){
            if(!blockSize){
                qDebug()<<"2";
                if(sender->bytesAvailable() < sizeof(quint16))
                    break;
                in >> blockSize;
            }

            if(sender->bytesAvailable() < blockSize)
                break;




            QJsonObject str;
            QJsonValue val;
            qintptr descReceiver = 0;

            in >> str >> descReceiver;
            blockSize = 0;

            sendMessageClient(sender, str, descReceiver);
            break;
        }
    }
}

void Server::slotClientDisconnected()//доработать
{
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());

    sender->close();
    sender->deleteLater();//?
    mTcpSockets.remove(sender);

    clients.remove(sender->peerAddress().toString());

    QJsonObject preambMessage;
    preambMessage.insert("type", "send_clients");
    for (auto sok: mTcpSockets){
        sendMessageClient(sok, preambMessage);
    }
}

void Server::sendMessageClient(QTcpSocket *sender, const QJsonObject jData ,qintptr receiverDesc)
{
    data.clear();

    QTcpSocket* receiver = getReceiverSok(receiverDesc);
    QHash<QString, qintptr> uniqClients;

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    if(jData.value("type").toString() == "send_clients"){

        QHash<QString, qintptr>::iterator it;
        for(it = clients.begin(); it != clients.end(); ++it){
            if (sender->socketDescriptor() != it.value())
                uniqClients.insert(it.key(), it.value());
        }
        out << quint16(0) << jData << uniqClients;
        qDebug()<<"send_clients";
    }
    else
        out << quint16(0) << jData;
    out.device()->seek(0);
    out <<quint16(data.size() - sizeof(quint16));

    if(receiver && receiverDesc){
        receiver->write(data);
        return;
    }
    else {
        sender->write(data);
        qDebug()<<"такого сокета нет!";
    }
}

QTcpSocket *Server::getReceiverSok(qintptr desc)
{
    for (auto sok: mTcpSockets){
       if( sok->socketDescriptor() == desc){
           return sok;
       }
    }
    return nullptr;
}
