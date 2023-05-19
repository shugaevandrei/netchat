#pragma once
#define SHOWTYPE(str) protocol::toString(static_cast<PacketTypes::Types>(str.value("type").toInt()))
#include <QString>
#include <QJsonObject>

namespace PacketTypes {
Q_NAMESPACE
enum Types {
    Error,

    NotificatonSuccesIn,
    NotificatonSuccesReg,
    NotificatonSuccesAddContact,

    ChatMessage,

    Get_ServiceAutentification,

    ServiceRegistration,
    ServiceAddContact,

    Post_ServiceAutorisation,
    Post_ServiceAutentification,

    Post_ServiceContactList,
};
Q_ENUMS(Types)
}
namespace protocol {
QJsonObject formsPacket(PacketTypes::Types type, QString content = QString());
QString toString(PacketTypes::Types);
}
