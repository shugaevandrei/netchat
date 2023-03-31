#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_BACKEND","software");
    QGuiApplication a(argc, argv);

    Server server;
    QUrl source(QStringLiteral("qrc:/clientForm.qml"));
    QQmlApplicationEngine engine;
    //engine.rootContext()->setContextProperty()
    engine.load(source);



//    Server server;
//    Client client;
//    QQuickView view;
//    view.engine()->rootContext()->setContextProperty("client", &client);
//    view.setSource(source);
//    view.show();

    return a.exec();
}
