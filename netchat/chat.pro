QT += network quick qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

SOURCES += \
        client.cpp \
        contactmodel.cpp \
        dialogmodel.cpp \
        main.cpp

HEADERS += \
    client.h \
    contactmodel.h \
    dialogmodel.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qml.qrc
