#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include "client.h"
#include "dialogmodel.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_BACKEND","software");

    QGuiApplication a(argc, argv);
    QQmlApplicationEngine engine;

    Client client;

    engine.rootContext()->setContextProperty("client", &client);
    engine.rootContext()->setContextProperty("dialogModel", &client.getModel());
    engine.rootContext()->setContextProperty("contactModel", &client.getContactModel());

    const QUrl url("qrc:/clientForm.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &a, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    return a.exec();
}
