#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QQuickView>
#include <QDataStream>
#include <QJsonObject>
#include <QHash>
#include <QColor>

#include "dialogmodel.h"
#include "contactmodel.h"

enum class typeMessage{getClients, sendMessage};
class Client : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)//пример
    Q_INVOKABLE void newConnection(const QString &ip, const quint16 &port);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void postMessage(const QString &msg, const QString &type = "sendMessage");
    Q_INVOKABLE void addContact(const QString &cont);

    explicit Client(QObject *parent = nullptr);

    QString userName();
    void setUserName(const QString &userName);
    DialogModel& getModel();
    ContactModel& getContactModel();
    void getClients();

private slots:
    void onRedyRead();
    void onConnected();
    void onDisconnected();
signals:
    void userNameChanged();
private:
    DialogModel messageModel;
    ContactModel contactModel;
    QTcpSocket * mTcpSocket;
    QString m_userName = "BOBA";
    QByteArray data;
    qint16 blockSize;
};

#endif // CLIENT_H
