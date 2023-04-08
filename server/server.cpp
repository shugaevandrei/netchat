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
        connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
        connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);

        //оповещаем о новом контакте других участников чата
        preambMessage.insert("type", "send_clients");
        for (auto sok: mTcpSockets){
            sendMessageClient(sok, preambMessage);
        }
        sendMessageClient(mTcpSocket, preambMessage);

        mTcpSockets.insert(mTcpSocket);
        clients.insert(mTcpSocket->peerAddress().toString(), mTcpSocket->socketDescriptor());
    }
}

void Server::slotServerRead()
{
    qDebug()<<"slotServerRead";
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream in(sender);
    in.setVersion(QDataStream::Qt_6_4);
    qDebug()<<"in.status()"<<in.status();
    if(in.status() ==QDataStream::Ok){
        for(;;){
            if(!blockSize){
                qDebug()<<"2";
                if(sender->bytesAvailable() < sizeof(quint16))
                    break;
                in >> blockSize;
            }
            qDebug()<<"blockSize"<< blockSize;
            qDebug()<<sender->bytesAvailable();

            if(sender->bytesAvailable() < blockSize){
                qDebug()<<"XNJ&";
;                break;
            }


            qDebug()<<"4";
            QJsonObject str;
            QJsonValue val;
            qintptr descReceiver = 0;
            qDebug()<<"123123132" <<descReceiver;
            in >> str >> descReceiver;
            blockSize = 0;
            qDebug()<<"qintptr" <<descReceiver;

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


    qDebug()<<"receiver"<<receiver<<"receiverDesc"<<receiverDesc;
    if(receiver && receiverDesc){
        receiver->write(data);
        return;
    }
    else {
        sender->write(data);
        qDebug()<<"такого сокета нет!";
    }
    //sender->write(data);
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
