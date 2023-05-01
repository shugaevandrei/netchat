#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QQuickStyle>
#include "client.h"
#include "protocol.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_BACKEND","software");
    qputenv("QML_XHR_ALLOW_FILE_READ", QByteArray("1"));
    QQuickStyle::setStyle("Windows");

    QGuiApplication a(argc, argv);
    QQmlApplicationEngine engine;

    Client client;

    qmlRegisterUncreatableMetaObject(
                FilterTypes::staticMetaObject,
                "FiltersTypes", 1, 0, "FilterTypes",
                "Access to enums & flags only");
    qmlRegisterUncreatableMetaObject(
                PacketTypes::staticMetaObject,
                "PacketsTypes", 1, 0, "PacketTypes",
                "Access to enums & flags only");

    engine.rootContext()->setContextProperty("client", &client);
    engine.rootContext()->setContextProperty("dialogModel", &*client.getDialogModel());
    engine.rootContext()->setContextProperty("contactModel", &*client.getContactModel());

    const QUrl url("qrc:/client.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &a, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    return a.exec();
}
