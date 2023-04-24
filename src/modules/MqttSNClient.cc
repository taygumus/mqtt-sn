#include "MqttSNClient.h"
#include "messages/MqttSNMessage.h"
#include "types/MsgType.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client Works! \n";

    const auto& message = inet::makeShared<MqttSNMessage>();

    message->setLength(2);
    message->setMsgType(MsgType::ADVERTISE);

    //inet::Packet *packet = new inet::Packet("Pacchetto");
    //packet->insertAtBack(message);
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    //
}

} /* namespace mqttsn */
