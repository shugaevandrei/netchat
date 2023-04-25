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
    users.insert("admin", "admin");
}

void Server::slotNewConnection()
{
    QTcpSocket  *mTcpSocket = mTcpServer->nextPendingConnection();
    QJsonObject preambMessage;
    //создаем запрос учетных данных
    connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);

    mTcpSockets.insert(mTcpSocket);
    clients.insert(mTcpSocket->peerAddress().toString(), mTcpSocket->socketDescriptor());

    //preambMessage.insert("type", "connect_success");
    //sendMessageClient(mTcpSocket, preambMessage);

//
    //preambMessage.insert("type", "get_accountingdata");
    //sendMessageClient(mTcpSocket, preambMessage);
//

//    if(!mTcpSockets.contains(mTcpSocket)){
//        connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
//        connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);

//        mTcpSockets.insert(mTcpSocket);
//        clients.insert(mTcpSocket->peerAddress().toString(), mTcpSocket->socketDescriptor());

//        preambMessage.insert("type", "send_clients");
//        for (auto sok: mTcpSockets){
//            sendMessageClient(sok, preambMessage);
//        }
//    }
}

void Server::slotServerRead()
{
    qDebug()<<"slotServerRead";
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream in(sender);
    blockSize = 0;
    in.setVersion(QDataStream::Qt_6_4);
    if(in.status() ==QDataStream::Ok) {
        for(;;){
            if(!blockSize){
                if(sender->bytesAvailable() < sizeof(quint16))
                    break;
                in >> blockSize;
                qDebug()<<"blockSize"<<blockSize<<sender->bytesAvailable();
            }

            if(sender->bytesAvailable() < blockSize)
                break;




            QJsonObject str;
            QJsonValue val;
            qintptr descReceiver = 0;

            in >> str ;//>> descReceiver;
            qDebug()<<"тип сообщения на сервере"<<str.value("type").toString();
            if (str.value("type").toString() == "send_accountingdata") {
                QString login, pass;
                in >> login >> pass;
                qDebug()<<login<<pass;
                //проверка полученных данных
                checkAccData(sender,login, pass);
                return;
            }
            else if (str.value("type").toString() == "registration") {
                QString login, pass, type;
                in >> login >> pass;// >> type;
                qDebug()<<login<<pass<<type;
                registration(login , pass);
                return;
            }
            else
                in >> descReceiver;
            //blockSize = 0;
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

bool Server::registration(const QString &log, const QString &pass)
{
    QHash<QString, QString>::iterator it;
    for(it = users.begin(); it != users.end(); ++it){
        if(it.key() == log){
            //такой пользователь уже есть
            qDebug()<<"плохо все, такой есть уже";
            return false;
        }
    }
    qDebug()<<"ЗАРЕГАЛСЯ";
    users.insert(log, pass);
    return true;

}

bool Server::checkAccData(QTcpSocket *sender, const QString &log, const QString &pass)
{
    QJsonObject sendClientsMess;
    QHash<QString, QString>::iterator it;
    for(it = users.begin(); it != users.end(); ++it){
        if(it.key() == log && it.value() == pass) {
            qDebug()<<"ВХОД УСПЕШЕН";

            QJsonObject preambMessage;
            preambMessage.insert("type", "succesIn");
            qDebug()<<preambMessage.value("type");
            sendMessageClient(sender, preambMessage);//отправка подтверждения успешного входа

            //отправлем список клиентов
           // preambMessage.remove("type");
            //qDebug()<<preambMessage.value("type");

            sendClientsMess.insert("type", "send_clients");
            qDebug()<<sendClientsMess.value("type");
            for (const auto &sok: qAsConst(mTcpSockets)){
                sendMessageClient(sok, sendClientsMess);
            }
            //
            return true;
        }
    }
    qDebug()<<"ВХОД НЕ УСПЕШЕН!";
    sendClientsMess.insert("type", "unsuccessIn");
    sendMessageClient(sender, sendClientsMess);
    return false;
}

void Server::sendMessageClient(QTcpSocket *sender, const QJsonObject jData ,qintptr receiverDesc)
{
    data.clear();
    qDebug()<<"jData.value.toString()"<<jData.value("type").toString()<<"?";
    QTcpSocket* receiver = getReceiverSok(receiverDesc);
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    //формирование уникального списка контакнтов для каждого пользователя
    if(jData.value("type").toString() == "send_clients"){
        QHash<QString, qintptr> uniqClients;
        QHash<QString, qintptr>::iterator it;
        for(it = clients.begin(); it != clients.end(); ++it){
            if (sender->socketDescriptor() != it.value())
                uniqClients.insert(it.key(), it.value());
        }
        out << quint16(0) << jData << uniqClients;
        qDebug()<<"send_clients";
    }
    else if (jData.value("type").toString() == "get_accountingdata"
             /*|| jData.value("type").toString() == "connect_success"*/) {
        out << quint16(0) << jData;

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
