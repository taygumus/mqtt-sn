#include "MqttSNClient.h"
#include "messages/MqttSNUnsubscribe.h"
#include "types/MsgType.h"
#include "types/TopicIdType.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client is sending a new packet..\n";

    const auto& payload = inet::makeShared<MqttSNUnsubscribe>();
    payload->setMsgType(MsgType::UNSUBSCRIBE);
    payload->setTopicIdTypeFlag(TopicIdType::PRE_DEFINED_TOPIC_ID);

    EV << payload->getLength() << std::endl;
    EV << (int) payload->getTopicIdTypeFlag() << std::endl;

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
