#include "server.h"
#include <QDebug>
#include <QCoreApplication>

Server::Server(QObject *parent) : QObject(parent)
{
    tcpServer_ = new QTcpServer(this);

    connect(tcpServer_, &QTcpServer::newConnection, this, &Server::slotNewConnection);

    if(!tcpServer_->listen(QHostAddress::Any, 6000)) {
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started";
    }
    blockSize_ = 0;

    clients_.insert("2222",2323);
    clients_.insert("5455",275323);
    clients_.insert("6532",78);
    users_.insert("admin", "admin");
}

void Server::slotNewConnection()
{
    QTcpSocket  *mTcpSocket = tcpServer_->nextPendingConnection();
    QJsonObject preambMessage;

    connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);

    tcpSockets_.insert(mTcpSocket);
    clients_.insert(mTcpSocket->peerAddress().toString(), mTcpSocket->socketDescriptor());
}

void Server::slotServerRead()
{
    qDebug()<<"slotServerRead";
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream in(sender);
    blockSize_ = 0;
    in.setVersion(QDataStream::Qt_6_4);
    if(in.status() ==QDataStream::Ok) {
        for(;;){
            if(!blockSize_){
                if(sender->bytesAvailable() < sizeof(quint16))
                    break;
                in >> blockSize_;
                qDebug()<<"blockSize"<<blockSize_<<sender->bytesAvailable();
            }

            if(sender->bytesAvailable() < blockSize_)
                break;

            QJsonObject preamb;
            qintptr descReceiver = 0;

            in >> preamb ;
            switch (preamb.value("type").toInt()) {
            case static_cast<int>(PacketTypes::Types::Post_ServiceAutentification):
            {
                QString login, pass;
                in >> login >> pass;
                _checkAccData(sender,login, pass);
                return;
            }
                break;
            case static_cast<int>(PacketTypes::Types::ServiceRegistration):
            {
                QString login, pass;
                in >> login >> pass;
                _registration(sender, login , pass);
                return;
            }
                break;
            default:
                in >> descReceiver;
                break;
            }

            _sendMessageClient(sender, PacketTypes::Types::ChatMessage, preamb.value("content").toString(), descReceiver);
            break;
        }
    }
}

void Server::slotClientDisconnected()//доработать
{
    QTcpSocket  *sender = static_cast<QTcpSocket*>(QObject::sender());

    sender->close();
    sender->deleteLater();//?
    tcpSockets_.remove(sender);

    clients_.remove(sender->peerAddress().toString());

    QJsonObject preambMessage;
    for (auto sok: qAsConst(tcpSockets_))
        _sendMessageClient(sok, PacketTypes::Types::Post_ServiceContactList);
}

bool Server::_registration(QTcpSocket *sender, const QString &log, const QString &pass)
{
    QHash<QString, QString>::iterator it;
    for(it = users_.begin(); it != users_.end(); ++it){
        if(it.key() == log){
            //такой пользователь уже есть
            _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesReg, "0");
            return false;
        }
    }
    _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesReg, "1");
    users_.insert(log, pass);
    return true;
}

bool Server::_checkAccData(QTcpSocket *sender, const QString &log, const QString &pass)
{
    QHash<QString, QString>::iterator it;
    for(it = users_.begin(); it != users_.end(); ++it){
        if(it.key() == log && it.value() == pass) {
            _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesIn, "1");
            for (const auto &sok: qAsConst(tcpSockets_))
                _sendMessageClient(sok, PacketTypes::Types::Post_ServiceContactList);
            return true;
        }
    }
    _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesIn, "0");
    return false;
}

void Server::_sendMessageClient(QTcpSocket *sender, const PacketTypes::Types &type, const QString &msg, qintptr receiverDesc)
{
    data_.clear();
    QTcpSocket* receiver = _getReceiverSok(receiverDesc);
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);

    QJsonObject packet  = protocol::formsPacket(type, msg);
    //формирование уникального списка контакнтов для каждого пользователя

    if(type == PacketTypes::Types::Post_ServiceContactList){
        QHash<QString, qintptr> uniqClients;
        QHash<QString, qintptr>::iterator it;
        for(it = clients_.begin(); it != clients_.end(); ++it){
            if (sender->socketDescriptor() != it.value())
                uniqClients.insert(it.key(), it.value());
        }
        out << quint16(0) << packet << uniqClients;
    }
    else if (type == PacketTypes::Types::Get_ServiceAutentification)
        out << quint16(0) << packet;
    else
        out << quint16(0) << packet;

    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));

    if(receiver && receiverDesc){
        receiver->write(data_);
        return;
    }
    else
        sender->write(data_);
}

QTcpSocket *Server::_getReceiverSok(qintptr desc)
{
    for (auto sok: qAsConst(tcpSockets_)) {
        if( sok->socketDescriptor() == desc){
            return sok;
        }
    }
    return nullptr;
}
