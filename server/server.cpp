#include "server.h"
#include <QDebug>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QList>

Server::Server(QObject *parent) : QObject(parent)
{
    tcpServer_ = new QTcpServer(this);

    connect(tcpServer_, &QTcpServer::newConnection, this, &Server::slotNewConnection);

    if(!tcpServer_->listen(QHostAddress::Any, 6000)) {
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started"<<tcpServer_->serverError();
    }

    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName("C:\\git\\netChat\\server\\users.sqlite");
    if(!db_.open())
        qDebug()<<db_.lastError().text();



   // clients_.insert("2222",2323);
   // clients_.insert("5455",275323);
   // clients_.insert("6532",78);
   // users_.insert("admin", "admin");
}

void Server::slotNewConnection()
{
    qDebug()<<"нОВЫЙ КЛИТЕНТ";
    QTcpSocket  *mTcpSocket = tcpServer_->nextPendingConnection();
    QJsonObject preambMessage;

    connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);

    tcpSockets_.insert(mTcpSocket, QString());
    //clients_.insert(mTcpSocket->peerAddress().toString(), mTcpSocket->socketDescriptor());//?????скорее всего не нужно!
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
           // qintptr descReceiver = 0;
            QString receiver = 0;

            in >> preamb ;
            switch (preamb.value("type").toInt()) {
            case static_cast<int>(PacketTypes::Types::Post_ServiceAutentification):
            {
                QString login, pass;
                in >> login >> pass;
                tcpSockets_[sender] = login;//добавляем логин к сокету
                qDebug()<<tcpSockets_.values();
                _checkAccData(sender, login, pass);
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
            case static_cast<int>(PacketTypes::Types::ServiceAddContact):
            {
                QString login = preamb.value("content").toString();

                _addNewContact(sender, login);

                return;
            }
                break;
            default:
                //in >> descReceiver;
                in >> receiver;
                break;
            }

            _sendMessageClient(sender, PacketTypes::Types::ChatMessage, preamb.value("content").toString(), receiver/*descReceiver*/);
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

    //clients_.remove(sender->peerAddress().toString());

    QJsonObject preambMessage;
    for (const auto &sok: tcpSockets_.keys())
        _sendMessageClient(sok, PacketTypes::Types::Post_ServiceContactList);
}

bool Server::_registration(QTcpSocket *sender, const QString &log, const QString &pass)
{
//----------------
    QSqlQuery query1("SELECT login FROM users;");
    QSqlRecord rec = query1.record();

    QString login;

    while(query1.next()){
        login = query1.value(rec.indexOf("login")).toString();

        if(login == log) {
            _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesReg, "0");
            return false;
        }
    }

//---------------- неактуально
//    QHash<QString, QString>::iterator it;
//    for(it = users_.begin(); it != users_.end(); ++it){
//        if(it.key() == log){
//            //такой пользователь уже есть
//            _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesReg, "0");
//            return false;
//        }
//    }
//    _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesReg, "1");
//    users_.insert(log, pass);
//--------неактуально

    QSqlQuery query;
    query.prepare("INSERT INTO users (login, password)"
                  "VALUES (:login, :password)");
    query.bindValue(":login", log);
    query.bindValue(":password", pass);
    query.exec();

    query.prepare("INSERT INTO contacts (login)"
                  "VALUES (:login)");
    query.bindValue(":login", log);
    query.exec();

    _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesReg, "1");
    return true;
//--------
}

bool Server::_checkAccData(QTcpSocket *sender, const QString &log, const QString &pass)
{
    //--------
            QSqlQuery query("SELECT * FROM users;");
            QSqlRecord rec = query.record();

            QString login, password;

            while(query.next()){
                login = query.value(rec.indexOf("login")).toString();
                password = query.value(rec.indexOf("password")).toString();

                if (login == log && password == pass) {
                    _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesIn, "1");
                    for (const auto &sok: tcpSockets_.keys())
                        _sendMessageClient(sok, PacketTypes::Types::Post_ServiceContactList);
                    return true;
                }

            }
    //--------
//    QHash<QString, QString>::iterator it;
//    for(it = users_.begin(); it != users_.end(); ++it){
//        if(it.key() == log && it.value() == pass) {
//            _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesIn, "1");
//            for (const auto &sok: qAsConst(tcpSockets_))
//                _sendMessageClient(sok, PacketTypes::Types::Post_ServiceContactList);
//            return true;
//        }
//    }
    _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesIn, "0");
    return false;
}

void Server::_sendMessageClient(QTcpSocket *sender, const PacketTypes::Types &type, const QString &msg, QString rec/*qintptr receiverDesc*/)
{
    data_.clear();
    QTcpSocket* receiver = _getReceiverSok(rec);
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);

    QJsonObject packet  = protocol::formsPacket(type, msg);
    //формирование уникального списка контакнтов для каждого пользователя

    if(type == PacketTypes::Types::Post_ServiceContactList){
        QHash<QString, bool> uniqClients;//логин, статус (on/off)
      //  QHash<QString, qintptr> uniqClients;
//        QHash<QString, qintptr>::iterator it;

//        for(it = clients_.begin(); it != clients_.end(); ++it){
//            if (sender->socketDescriptor() != it.value())
//                uniqClients.insert(it.key(), it.value());
//        }


        QString senderLogin = tcpSockets_[sender];
        QString strQuery = QString("SELECT contactLogin FROM contacts c LEFT JOIN usersContacts uc ON c.login  = uc.userLogin WHERE login = '%1';").arg(senderLogin);
        QSqlQuery query(strQuery);

        QSqlRecord rec = query.record();

        QString login;
        QList<QString> onlineUsers = tcpSockets_.values();

        while(query.next()){
            login = query.value(rec.indexOf("contactLogin")).toString();
            if (onlineUsers.contains(login))
                uniqClients.insert(login, 1);
            else
                uniqClients.insert(login, 0);
        }

        out << quint16(0) << packet << uniqClients;
    }
    else if (type == PacketTypes::Types::Get_ServiceAutentification)
        out << quint16(0) << packet;
    else
        out << quint16(0) << packet;

    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));

    if(receiver && rec != "-1"){
        receiver->write(data_);
        return;
    }
    else
        sender->write(data_);
}

//QTcpSocket *Server::_getReceiverSok(qintptr desc)
//{
//    for (const auto &sok: tcpSockets_.keys()) {
//        if (sok->socketDescriptor() == desc) {
//            return sok;
//        }
//    }
//    return nullptr;
//}

QTcpSocket *Server::_getReceiverSok(const QString &login)
{
    for (const auto &client: tcpSockets_.values()) {
        if (client == login) {
            return tcpSockets_.key(client);
        }
    }
    return nullptr;
}


void Server::_addNewContact(QTcpSocket *sender, const QString &login)
{

    QSqlQuery query("SELECT login FROM contacts;");
    QSqlRecord rec = query.record();

    QString newContLogin;

    while(query.next()){
        newContLogin = query.value(rec.indexOf("login")).toString();

        if (newContLogin == login) {
            QSqlQuery query;
            query.prepare("INSERT INTO usersContacts (userLogin, contactLogin)"
                          "VALUES (:userLogin, :contactLogin)");
            query.bindValue(":userLogin", tcpSockets_[sender]);
            query.bindValue(":contactLogin", newContLogin);
            query.exec();
            qDebug()<<"новый контакт успешно добавлен";
            _sendMessageClient(sender, PacketTypes::Types::Post_ServiceContactList);
            _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesAddContact, "1");
            return;
        }
    }
    _sendMessageClient(sender, PacketTypes::Types::NotificatonSuccesAddContact, "0");
}
