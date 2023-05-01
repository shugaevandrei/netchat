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
INCLUDEPATH = "C:/git/netChat/chat/protocol"
LIBS = "C:/git/netChat/chat/build-protocol-Felgo_SDK_Desktop_Qt_6_4_1_MinGW_64_bit-Debug/debug/libprotocol.a"
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qml.qrc
