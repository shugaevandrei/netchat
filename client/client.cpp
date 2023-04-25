#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent},
     filterModel(new ProxyFilterModel)
{
    mTcpSocket = new QTcpSocket(this);
    connect(mTcpSocket, &QAbstractSocket::readyRead,this ,&Client::onRedyRead);
    connect(mTcpSocket, &QAbstractSocket::connected,this ,&Client::onConnected);
    connect(mTcpSocket, &QAbstractSocket::disconnected,this ,&Client::onDisconnected);
    blockSize = 0;
    filterModel->setSourceModel(&messageModel);
    readDialogs();
}

Client::~Client()
{
    saveDialogs();
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
            if(jMessage.value("type").toString() == "send_clients") {
                clients.clear();
                contactModel.clearAll();
                in >> clients;

                for(auto client: clients.keys())
                    contactModel.add(client);
            }
            else if(jMessage.value("type").toString() == "get_accountingdata"){
                postMessage(QString(), "-1", "send_accountingdata");
            }
            else if(jMessage.value("type").toString() == "succesIn"){
                autorisation_ = true;
                emit isAutorisationChanged();
                //postMessage(QString(), "-1", "send_accountingdata");
            }
            else if(jMessage.value("type").toString() == "unsuccessIn") {
                autorisation_ = false;//убрать?
                emit isAutorisationChanged();//убрать?
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
    connect_ = true;
    emit isConnectChanged();
    qDebug()<<"onConnected";
}

void Client::onDisconnected()
{
    connect_ = false;
    autorisation_ = false;
    emit isConnectChanged();
    emit isAutorisationChanged();
    qDebug()<<"onDisconnected";
}

bool Client::isConnect()
{
    return connect_;
}

bool Client::isAutorisation()
{
    return autorisation_;
}

ContactModel &Client::getContactModel()
{
    return contactModel;
}

void Client::saveDialogs()
{
    messageModel.saveCurrentModel();

    QFile out("dialogs.bin");
    if( out.open( QIODevice::WriteOnly )) {
        QDataStream stream( &out );
        stream << messageModel.getModel();
        out.close();
    }
}

void Client::readDialogs()
{
    QFile in("dialogs.bin");
    if( in.open( QIODevice::ReadOnly )) {
        QHash<QString, QList<Message>> model;
        QDataStream stream( &in );
        stream >> model;
        messageModel.setModel(model);
        in.close();
    }
}

quintptr Client::getReceiver(const QString &name)
{
    for(auto val: clients.keys()){
        if(name == val)
            return clients[val];
    }
    return -1;
}

void Client::autorisationToServer(const QString &login, const QString &pass, const QString &type)
{
    accData.login = login;
    accData.pass = pass;


    if (type == "reg")
        postMessage(QString(), "-1", "registration");
    else
        postMessage(QString(), "-1", "send_accountingdata");
}

void Client::toConnect(const QString &ip, const quint16 &port)
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
    QString addMess = QDateTime::currentDateTime().toString("[dd.MM hh:mm:ss] ") + msg;
    data.clear();

    jMessage.insert("type", type);
    jMessage.insert("message", addMess);

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);

    if(type == "sendMessage") {
        quintptr descReceiver = getReceiver(receiver);
        if (descReceiver)
            out <<quint16(0)<< jMessage << descReceiver;
        messageModel.add(addMess, "blue");
    }
    else if(type == "send_accountingdata"
         || type == "registration") {
        //отправка учетных данных
        out <<quint16(0)<< jMessage << accData.login<<accData.pass;
    }
    else {

        qDebug()<<"Ошибка передачи сообщения!";
        return;
    }

    out.device()->seek(0);
    out <<quint16(data.size() - sizeof(quint16));
    mTcpSocket->write(data);

}

void Client::addContact(const QString &cont)
{
    contactModel.add(cont);
}

void Client::setCurReceiver(const QString &interlocutor)
{
    messageModel.setModel(interlocutor);
}

void Client::applyFilter(const FilterTypes::Filter &typeFilter, const QString &key)
{
    filterModel->setFilterKind(typeFilter);
    filterModel->updateFilter(key);
}

QScopedPointer<ProxyFilterModel> &Client::getModel()
{
    return filterModel;
}
