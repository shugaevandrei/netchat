#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    mTcpSocket = new QTcpSocket(this);
    connect(mTcpSocket, &QAbstractSocket::readyRead,this ,&Client::onRedyRead);
    connect(mTcpSocket, &QAbstractSocket::connected,this ,&Client::onConnected);
    connect(mTcpSocket, &QAbstractSocket::disconnected,this ,&Client::onDisconnected);
}
void Client::onRedyRead(){
    qDebug()<<"readyRead";
    qDebug()<< mTcpSocket->readAll();
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
void Client::newConnection()
{
    mTcpSocket->connectToHost(QHostAddress::LocalHost, 6000);
}

void Client::postMessage(const QString &msg)
{
   messageModel.add();
}

