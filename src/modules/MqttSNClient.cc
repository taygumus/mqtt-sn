#include "MqttSNClient.h"
#include "messages/MqttSNSubscribe.h"
#include "types/TopicIdType.h"
#include "types/MsgType.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client is sending a new packet..\n";

    const auto& payload = inet::makeShared<MqttSNSubscribe>();
    payload->setMsgType(MsgType::SUBSCRIBE);

    //payload->setTopicIdTypeFlag(TopicIdType::PRE_DEFINED_TOPIC_ID);
    //payload->setTopicId(25);

    EV << payload->getLength() << std::endl;
    //EV << payload->getTopicId() << std::endl;

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
