QT += network core sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11 console

SOURCES += \
        main.cpp \
        server.cpp

HEADERS += \
    server.h

INCLUDEPATH = "C:/git/netChat/protocol"
LIBS = "C:/git/netChat/build-protocol-Felgo_SDK_Desktop_Qt_6_4_1_MinGW_64_bit-Debug/debug/libprotocol.a"


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
