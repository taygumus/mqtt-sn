#include "MqttSNClient.h"
#include "messages/MqttSNWillTopic.h"
#include "types/MsgType.h"
#include "types/QoS.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client is sending a new packet..\n";

    const auto& payload = inet::makeShared<MqttSNWillTopic>();

    payload->setMsgType(MsgType::WILLTOPIC);
    payload->setQoSFlag(QoS::QOS_0);
    payload->setRetainFlag(true);

    EV << (int) payload->getQoSFlag() << std::endl;
    EV << payload->getRetainFlag() << std::endl;

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
