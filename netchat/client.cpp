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
        qDebug()<<"clients size1"<<clients.size();
        while(1){
            if(blockSize == 0){
                if(mTcpSocket->bytesAvailable() < 2)
                    break;
                in >> blockSize;
                qDebug()<<"clients size2"<<clients.size();
            }
            if(mTcpSocket->bytesAvailable() < blockSize)
                break;

            QJsonObject jMessage;
            //clients.clear();//зачем?
            qDebug()<<"clients size13"<<clients.size();

            in >> jMessage>>clients; ;
            //qDebug()<<"clients size14"<<clients.size();
            blockSize = 0;

            if(jMessage.value("type").toString() == "send_clients"){
                //in >> jMessage >> clients;
                //contactModel.clearAll();
                for(auto clients: clients.keys())
                    contactModel.add(clients);
            }
            else {
                //in >> jMessage;
                messageModel.add(jMessage.value("message").toString(), "red");
            }
        }
        qDebug()<<"clients size5"<<clients.size();
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

int Client::getReceiver(const QString &name)
{
    qDebug()<<"name"<<name;
    qDebug()<<"clients size !"<<clients.size();
    for(QString val: clients.keys()){
        qDebug()<<"val"<<val;
        if(name == val){
            qDebug()<<"val goal"<<val;
            return clients[val];
        }

   }
    return -1;
    //return 100;
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
       // int descReceiver = getReceiver(receiver);
        //if (descReceiver) {
            out <<quint16(0)<< jMessage << 1;
       // }
    }
    else {
        qDebug()<<"Ошибка передачи сообщения!";
        return;
    }
    //qintptr desc = 1;
    qDebug()<<"clients size"<<clients.size();
    //out <<quint16(0)<< jMessage << desc;
    out.device()->seek(0);
    out <<quint16(data.size() - sizeof(quint16));
    mTcpSocket->write(data);
    messageModel.add(msg, "blue");

}

void Client::addContact(const QString &cont)
{
    contactModel.add(cont);
}

