#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QQuickView>

#include "dialogmodel.h"

class Client : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_INVOKABLE void newConnection();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void postMessage(const QString &msg);

    explicit Client(QObject *parent = nullptr);

    QString userName();
    void setUserName(const QString &userName);
    DialogModel& getModel();

private slots:
    void onRedyRead();
    void onConnected();
    void onDisconnected();
signals:
    void userNameChanged();
private:
    DialogModel messageModel;
    QTcpSocket * mTcpSocket;
    QString m_userName = "BOBA";
};

#endif // CLIENT_H
