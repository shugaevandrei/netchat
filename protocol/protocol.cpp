#include "protocol.h"

QJsonObject protocol::formsPacket(PacketTypes::Types type, QString content) {
    QJsonObject packet;
    packet.insert("type", static_cast<int>(type));
    packet.insert("content", content);
    return packet;
};

QString protocol::toString(PacketTypes::Types type)
{
    switch(type) {
    case PacketTypes::Types::ChatMessage :
        return "ChatMessage";
    break;
    case PacketTypes::Types::Error :
        return "Error";
    break;
    case PacketTypes::Types::Get_ServiceAutentification :
        return "Get_ServiceAutentification";
    break;
    case PacketTypes::Types::Post_ServiceAutentification :
        return "Post_ServiceAutentification";
    break;
    case PacketTypes::Types::NotificatonSuccesIn :
        return "NotificatonSuccesIn";
    break;
    case PacketTypes::Types::NotificatonSuccesReg :
        return "NotificatonSuccesReg";
    break;
    case PacketTypes::Types::Post_ServiceAutorisation :
        return "Post_ServiceAutorisation";
    break;
    case PacketTypes::Types::Post_ServiceContactList :
        return "Post_ServiceContactList";
    break;
    case PacketTypes::Types::ServiceRegistration:
        return "ServiceRegistration";
    break;
    default:
        return "ТАКОГО ТИПА СООБЩЕНИЯ НЕТ!";
        break;
    }
}
