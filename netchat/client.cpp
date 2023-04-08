#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    mTcpSocket = new QTcpSocket(this);
    connect(mTcpSocket, &QAbstractSocket::readyRead,this ,&Client::onRedyRead);
    connect(mTcpSocket, &QAbstractSocket::connected,this ,&Client::onConnected);
    connect(mTcpSocket, &QAbstractSocket::disconnected,this ,&Client::onDisconnected);
    blockSize = 0;
}
void Client::onRedyRead(){
    qDebug()<<"readyRead";

    QDataStream in(mTcpSocket);
    in.setVersion(QDataStream::Qt_6_4);

    if(in.status() == QDataStream::Ok){
        while(1){
            if(!blockSize){
                if(mTcpSocket->bytesAvailable() < sizeof(quint16))
                    break;
                in >> blockSize;
            }
            if(mTcpSocket->bytesAvailable() < blockSize)
                break;



            QJsonObject jMessage;
            in >> jMessage;

            if(jMessage.value("type").toString() == "send_clients"){
                clients.clear();
                contactModel.clearAll();
                in >> clients;

                for(auto client: clients.keys())
                    contactModel.add(client);
            }
            else
                messageModel.add(jMessage.value("message").toString(), "red");
            blockSize = 0;
        }
    }
    else {
        //обработка ошибок
    }
}

void Client::onConnected()
{
    qDebug()<<"onConnected";
}

void Client::onDisconnected()
{
    qDebug()<<"onDisconnected";
}

QString Client::userName()
{
    return m_userName;
}

void Client::setUserName(const QString &userName)
{
    if (userName == m_userName)
        return;

    m_userName = userName;
    emit userNameChanged();
}

DialogModel &Client::getModel()
{
    return messageModel;
}

ContactModel &Client::getContactModel()
{
    return contactModel;
}

quintptr Client::getReceiver(const QString &name)
{
    for(auto val: clients.keys()){
        if(name == val){
            return clients[val];
        }
    }
    return -1;
}

void Client::newConnection(const QString &ip, const quint16 &port)
{
    mTcpSocket->connectToHost(ip, port);
}

void Client::disconnect()
{
    mTcpSocket->disconnectFromHost();
    contactModel.clearAll();
}

void Client::postMessage(const QString &msg, const QString &receiver ,const QString &type)
{
    QJsonObject jMessage;
    data.clear();

    jMessage.insert("type", type);
    jMessage.insert("message", msg);

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);

    if(type == "sendMessage") {
        quintptr descReceiver = getReceiver(receiver);
        if (descReceiver)
            out <<quint16(0)<< jMessage << descReceiver;
    }
    else {
        qDebug()<<"Ошибка передачи сообщения!";
        return;
    }

    out.device()->seek(0);
    out <<quint16(data.size() - sizeof(quint16));
    mTcpSocket->write(data);
    messageModel.add(msg, "blue");
}

void Client::addContact(const QString &cont)
{
    contactModel.add(cont);
}

