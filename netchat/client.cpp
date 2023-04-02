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
            if(blockSize == 0){
                if(mTcpSocket->bytesAvailable() < 2)
                    break;
                in >> blockSize;
            }
            if(mTcpSocket->bytesAvailable() < blockSize)
                break;

            QJsonObject jMessage;
            QHash<QString, quint16> hash;

            in >> jMessage >> hash;
            blockSize = 0;

            if(jMessage.value("type").toString() == "send_clients"){
                //тут очистить модель клиентоа
                qDebug()<<"MODEL";
                for(auto clients: hash.keys()){
                    contactModel.add(clients);
                }
            }
            else
                messageModel.add(jMessage.value("message").toString(), "red");
        }
    }
    else {
        //обработка ошибок
    }
}
void Client::onConnected(){
    qDebug()<<"onConnected";
}
void Client::onDisconnected(){
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

void Client::newConnection(const QString &ip, const quint16 &port)
{
    mTcpSocket->connectToHost(ip, port);
}

void Client::disconnect()
{
    mTcpSocket->disconnectFromHost();
}

void Client::postMessage(const QString &msg, const QString &type)
{
    QJsonObject jMessage;
    data.clear();

    jMessage.insert("message", msg);
    jMessage.insert("type", type);

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out <<quint16(0)<< jMessage;
    out.device()->seek(0);
    out <<quint16(data.size() - sizeof(quint16));
    mTcpSocket->write(data);
    messageModel.add(msg, "blue");
}

void Client::addContact(const QString &cont)
{
    contactModel.add(cont);
}

