#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    mTcpSocket = new QTcpSocket(this);
    connect(mTcpSocket, &QAbstractSocket::readyRead,this ,&Client::onRedyRead);
    connect(mTcpSocket, &QAbstractSocket::connected,this ,&Client::onConnected);
    connect(mTcpSocket, &QAbstractSocket::disconnected,this ,&Client::onDisconnected);

    //view.setInitialProperties({{ "model", QVariant::fromValue(dataList) }});
}
void Client::onRedyRead(){
    qDebug()<<"readyRead";
    messageModel.add(mTcpSocket->readAll());
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

void Client::newConnection()
{
    mTcpSocket->connectToHost(QHostAddress::LocalHost, 6000);
}

void Client::disconnect()
{
    mTcpSocket->disconnectFromHost();
}

void Client::postMessage(const QString &msg)
{
   messageModel.add(msg);
}

