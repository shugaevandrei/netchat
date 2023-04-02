QT += network core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11 console

SOURCES += \
        main.cpp \
        server.cpp

HEADERS += \
    server.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
