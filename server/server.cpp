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


//    QTcpSocket s1, s2, s3;
//    s1.connectToHost("127.0.0.1", 6000);
//    s2.connectToHost("127.0.0.1", 6000);
//    s3.connectToHost("127.0.0.1", 6000);
//    qDebug()<<s1.peerAddress().toString()<< s1.socketDescriptor();
//    clients.insert(s1.peerAddress().toString(), s1.socketDescriptor());
//    clients.insert(s2.peerAddress().toString(), s2.socketDescriptor());
//    clients.insert(s3.peerAddress().toString(), s3.socketDescriptor());

    clients.insert("2222",2323);
    clients.insert("5455",275323);
    clients.insert("6532",78);
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
        clients.insert(mTcpSocket->peerAddress().toString(), mTcpSocket->socketDescriptor());
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
            qintptr descReceiver = 0;

            in >> str >> descReceiver;
            qDebug()<<descReceiver;
            blockSize = 0;
            sendMessageClient(sender, str);
            break;
        }
    }
}

void Server::slotClientDisconnected()//доработать
{
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());

    qDebug()<<sender->peerAddress().toString();
    qDebug()<<sender->socketDescriptor();

    sender->close();
    sender->deleteLater();//?
    mTcpSockets.remove(sender);

    clients.remove(sender->peerAddress().toString());
    qDebug()<<mTcpSockets.size();
}

//void Server::findReceiver(qintptr ptr)
//{
//    for(qintptr val: clients.values()){
//        if(ptr == val)
//            qDebug()<<"YYY"
//           // return clients[val];

//    }
//    //return -1;
//}

void Server::sendMessageClient(QTcpSocket *sender, const QJsonObject jData)
{
    data.clear();
    //findReceiver(jData.value("receiver"));

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    if(jData.value("type").toString() == "send_clients"){
        out << quint16(0) << jData << clients;
        qDebug()<<"send_clients";
    }
    else
        out << quint16(0) << jData;
    out.device()->seek(0);
    out <<quint16(data.size() - sizeof(quint16));
    sender->write(data);
    qDebug()<<"DDDDDDD";
}
