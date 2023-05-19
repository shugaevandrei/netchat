#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent},
      messageModel_(new DialogModel),
      contactModel_(new ContactModel),
      filterModel_(new ProxyFilterModel)
{
    tcpSocket_ = new QTcpSocket(this);
    connect(tcpSocket_, &QAbstractSocket::readyRead, this, &Client::onRedyRead);
    connect(tcpSocket_, &QAbstractSocket::connected, this, &Client::onConnected);
    connect(tcpSocket_, &QAbstractSocket::disconnected, this, &Client::onDisconnected);
    connect(tcpSocket_, &QAbstractSocket::stateChanged, this, [this] {
        if (tcpSocket_->state() == QAbstractSocket::UnconnectedState) {
            unconnecting_ = true;
            emit isUnconnectingChanged();
        }
    });
    blockSize_ = 0;
    filterModel_->setSourceModel(&*messageModel_);
    readDialogs();
}

Client::~Client()
{
    saveDialogs();
}

void Client::onRedyRead(){
    qDebug()<<"readyRead";

    QDataStream in(tcpSocket_);
    in.setVersion(QDataStream::Qt_6_4);

    if(in.status() == QDataStream::Ok) {
        while(1) {
            if(!blockSize_) {
                if(tcpSocket_->bytesAvailable() < sizeof(quint16))
                    break;
                in >> blockSize_;
            }
            if(tcpSocket_->bytesAvailable() < blockSize_)
                break;

            QJsonObject preamb;
            in >> preamb;

            switch(preamb.value("type").toInt()) {
            case static_cast<int>(PacketTypes::Types::Post_ServiceContactList):
                conacts_.clear();
                contactModel_->clearAll();
                in >> conacts_;

                for(const auto &client: conacts_.keys())
                    contactModel_->add(ContactModel::User(client, conacts_[client]));
                break;
            case  static_cast<int>(PacketTypes::Types::Get_ServiceAutentification):
                postMessage(QString(), PacketTypes::Types::Post_ServiceAutentification);
                break;
            case static_cast<int>(PacketTypes::Types::NotificatonSuccesIn):
                autorisation_ = false;
                if (preamb.value("content").toString().toInt())
                    autorisation_ = true;
                emit isAutorisationChanged();
                break;
            case static_cast<int>(PacketTypes::Types::NotificatonSuccesReg):
                registration_ = false;
                if (preamb.value("content").toString().toInt())
                    registration_ = true;
                emit isRegistrationChanged();
                break;
            case static_cast<int>(PacketTypes::Types::NotificatonSuccesAddContact):
                addContact_ = false;
                if (preamb.value("content").toString().toInt())
                    addContact_ = true;
                emit isAddContactChanged();
                break;
            default:
                messageModel_->add(preamb.value("content").toString(), "red");
                break;
            }
            blockSize_ = 0;
        }
    }
    else {
        //обработка ошибок
    }
}

void Client::onConnected()
{
    connect_ = true;
    unconnecting_ = false;
    emit isConnectChanged();
    emit isUnconnectingChanged();
    qDebug()<<"onConnected";
}

void Client::onDisconnected()
{
    connect_ = false;
    autorisation_ = false;
    emit isConnectChanged();
    qDebug()<<"onDisconnected";
}

bool Client::isConnect()
{
    return connect_;
}

bool Client::isUnconnecting()
{
    return unconnecting_;
}

bool Client::isAutorisation()
{
    return autorisation_;
}

bool Client::isRegistration()
{
    return registration_;
}

bool Client::isAddContact()
{
    return addContact_;
}

QScopedPointer<ContactModel> &Client::getContactModel()
{
    return contactModel_;
}

void Client::saveDialogs()
{
    messageModel_->saveCurrentModel();

    QFile out("dialogs.bin");
    if(out.open( QIODevice::WriteOnly)) {
        QDataStream stream( &out );
        stream << messageModel_->getModel();
        out.close();
    }
}

void Client::readDialogs()
{
    QFile in("dialogs.bin");
    if(in.open( QIODevice::ReadOnly)) {
        QHash<QString, QList<Message>> model;
        QDataStream stream( &in );
        stream >> model;
        messageModel_->setModel(model);
        in.close();
    }
}

quintptr Client::getReceiver(const QString &name)
{
    for(const auto &val: conacts_.keys()) {
        if(name == val){}
            return conacts_[val];
    }
    return -1;
}

void Client::autorisation(const QString &login, const QString &pass)
{
    accData_.login = login;
    accData_.pass = pass;
    postMessage(QString(), PacketTypes::Types::Post_ServiceAutentification);
}

void Client::registration(const QString &login, const QString &pass)
{
    accData_.login = login;
    accData_.pass = pass;
    postMessage(QString(), PacketTypes::Types::ServiceRegistration);
}

void Client::toConnect(const QString &ip, const quint16 &port)
{
    tcpSocket_->connectToHost(ip, port);
}

void Client::disconnect()
{
    tcpSocket_->disconnectFromHost();
    contactModel_->clearAll();
}

void Client::postMessage(const QString &msg, const PacketTypes::Types &type, const QString &receiver)
{
    data_.clear();
    //QString addTimeMess = QDateTime::currentDateTime().toString("[dd.MM hh:mm:ss] ") + msg;
    QJsonObject packet  = protocol::formsPacket(type, msg);
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);

    switch (type) {
    case PacketTypes::Types::ChatMessage: {
        //quintptr descReceiver = getReceiver(receiver);
        if (receiver != "-1")
            out <<quint16(0)<< packet << receiver;
        messageModel_->add(msg, "blue");
    }
        break;
    case PacketTypes::Types::Post_ServiceAutentification:
    case PacketTypes::Types::ServiceRegistration:
        out <<quint16(0)<< packet << accData_.login<<accData_.pass;
        break;
    case PacketTypes::Types::ServiceAddContact:
        out <<quint16(0)<< packet;
        break;
    default:
        qDebug()<<"Ошибка передачи сообщения!";
        return;
        break;
    }

    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));
    tcpSocket_->write(data_);
}

void Client::addContact(const QString &cont)
{
    //contactModel_->add(cont);
     postMessage(cont, PacketTypes::Types::ServiceAddContact);
}

void Client::setCurReceiver(const QString &interlocutor)
{
    messageModel_->setModel(interlocutor);
}

void Client::applyFilter(const FilterTypes::Filter &typeFilter, const QString &key)
{
    filterModel_->setFilterKind(typeFilter);
    filterModel_->updateFilter(key);
}

QScopedPointer<ProxyFilterModel> &Client::getDialogModel()
{
    return filterModel_;
}
