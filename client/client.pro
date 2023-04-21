QT += network quick qml quickcontrols2
RC_ICONS = corn.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

SOURCES += \
        client.cpp \
        contactmodel.cpp \
        dialogmodel.cpp \
        main.cpp \
        proxyfiltermodel.cpp

HEADERS += \
    client.h \
    contactmodel.h \
    dialogmodel.h \
    proxyfiltermodel.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qml.qrc
