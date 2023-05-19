#pragma once

#include "dialogmodel.h"
#include "contactmodel.h"
#include "proxyfiltermodel.h"
#include "protocol.h"

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
    Q_PROPERTY (bool isRegistration READ isRegistration NOTIFY isRegistrationChanged)
    Q_PROPERTY (bool isUnconnecting READ isUnconnecting NOTIFY isUnconnectingChanged)
    Q_PROPERTY (bool isAddContact READ isAddContact NOTIFY isAddContactChanged)
    Q_INVOKABLE void autorisation(const QString &login, const QString &pass);
    Q_INVOKABLE void registration(const QString &login, const QString &pass);
    Q_INVOKABLE void toConnect(const QString &ip, const quint16 &port);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void postMessage(const QString &msg,
                                 const PacketTypes::Types &type = PacketTypes::Types::ChatMessage,
                                 const QString &receiver = "-1");
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
    bool isUnconnecting();
    bool isAutorisation();
    bool isRegistration();
    bool isAddContact();
    QScopedPointer<ProxyFilterModel> &getDialogModel();
    QScopedPointer<ContactModel> &getContactModel();
    void saveDialogs();
    void readDialogs();
    quintptr getReceiver(const QString &name);

private slots:
    void onRedyRead();
    void onConnected();
    void onDisconnected();

signals:
    void isConnectChanged();
    void isUnconnectingChanged();
    void isAutorisationChanged();
    void isRegistrationChanged();
    void isAddContactChanged();

private:
    QScopedPointer<DialogModel> messageModel_;
    QScopedPointer<ContactModel> contactModel_;
    QScopedPointer<ProxyFilterModel> filterModel_;
    QTcpSocket *tcpSocket_;
    bool connect_ = false;
    bool unconnecting_ = false;
    bool autorisation_  = false;
    bool registration_ = false;
    bool addContact_ = false;
    AccData accData_;
    QByteArray data_;
    qint16 blockSize_ = 0;
    //QHash<QString, qintptr> clients_;
    //QList<QString> conacts_;
    QHash<QString, bool> conacts_;
};
