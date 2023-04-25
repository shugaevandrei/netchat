#pragma once

#include "dialogmodel.h"
#include "contactmodel.h"
#include "proxyfiltermodel.h"

#include <QObject>
#include <QTcpSocket>
#include <QQuickView>
#include <QDataStream>
#include <QJsonObject>
#include <QHash>
#include <QColor>
#include <QDataStream>
#include <QFile>
#include <QDateTime>

class Client : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY (bool isConnect READ isConnect NOTIFY isConnectChanged)
    Q_PROPERTY (bool isAutorisation READ isAutorisation NOTIFY isAutorisationChanged)
    Q_INVOKABLE void autorisationToServer(const QString &login, const QString &pass, const QString &type  = "in");
    Q_INVOKABLE void toConnect(const QString &ip, const quint16 &port);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void postMessage(const QString &msg, const QString &receiver, const QString &type = "sendMessage");
    Q_INVOKABLE void addContact(const QString &cont);
    Q_INVOKABLE void setCurReceiver(const QString &interlocutor);
    Q_INVOKABLE void applyFilter (const FilterTypes::Filter &typeFilter, const QString &key = QString());

    explicit Client(QObject *parent = nullptr);
    ~Client();

    struct AccData{
        QString login;
        QString pass;
    };

    bool isConnect();
    bool isAutorisation();
    QScopedPointer<ProxyFilterModel> &getModel();
    ContactModel& getContactModel();
    void saveDialogs();
    void readDialogs();
    quintptr getReceiver(const QString &name);

private slots:
    void onRedyRead();
    void onConnected();
    void onDisconnected();

signals:
    void isConnectChanged();
    void isAutorisationChanged();

private:
    DialogModel messageModel;
    ContactModel contactModel;
    QScopedPointer<ProxyFilterModel> filterModel;
    QTcpSocket * mTcpSocket;
    bool connect_ = false;
    bool autorisation_  = false;
    AccData accData;
    QByteArray data;
    qint16 blockSize = 0;
    QHash<QString, qintptr> clients;
};
