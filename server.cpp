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
}

void Server::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();

    mTcpSocket->write("Hello, World!!! I am echo server!\r\n");

    connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);
}

void Server::slotServerRead()
{
    while(mTcpSocket->bytesAvailable()>0)
    {
        QByteArray array = mTcpSocket->readAll();

        mTcpSocket->write(array);
    }
}

void Server::slotClientDisconnected()
{
    mTcpSocket->close();
}
