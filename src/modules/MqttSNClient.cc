#include "MqttSNClient.h"
#include "messages/MqttSNConnect.h"
#include "types/MsgType.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client is sending a new packet..\n";

    const auto& payload = inet::makeShared<MqttSNConnect>();
    payload->setMsgType(MsgType::CONNECT);
    payload->setCleanSessionFlag(false);
    payload->setWillFlag(true);
    payload->setDuration(20);
    payload->setClientId("clientId");

    EV << payload->getCleanSessionFlag() << std::endl;
    EV << payload->getWillFlag() << std::endl;
    EV << payload->getDuration() << std::endl;
    EV << payload->getClientId() << std::endl;

    /*
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
    */
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    //
}

} /* namespace mqttsn */
