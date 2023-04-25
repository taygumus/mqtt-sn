#include "MqttSNClient.h"
#include "messages/MqttSNAdvertise.h"
#include "types/MsgType.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client is sending a new packet..\n";

    const auto& payload = inet::makeShared<MqttSNAdvertise>();

    payload->setMsgType(MsgType::ADVERTISE);
    payload->setGwId(0x02);
    payload->setDuration(10);

    uint16_t bytes = 4;
    payload->setLength(bytes);
    payload->setChunkLength(inet::B(bytes));

    std::ostringstream str;
    str << "Packet" << "-" << numSent;
    inet::Packet *packet = new inet::Packet(str.str().c_str());
    packet->insertAtBack(payload);

    inet::L3Address destAddr = chooseDestAddr();
    socket.sendTo(packet, destAddr, destPort);
    numSent++;
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    //
}

} /* namespace mqttsn */
