#include "MqttSNClient.h"
#include "messages/MqttSNSubAck.h"
#include "types/MsgType.h"
#include "types/QoS.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client is sending a new packet..\n";

    const auto& payload = inet::makeShared<MqttSNSubAck>();
    payload->setMsgType(MsgType::SUBACK);

    EV << payload->getLength() << std::endl;

    /*
    payload->setChunkLength(inet::B(payload->getLength()));

    std::ostringstream str;
    str << "Packet" << "-" << numSent;
    inet::Packet *packet = new inet::Packet(str.str().c_str());
    packet->insertAtBack(payload);

    inet::L3Address destAddr = chooseDestAddr();
    socket.sendTo(packet, destAddr, destPort);
    numSent++;
    */
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    //
}

} /* namespace mqttsn */
