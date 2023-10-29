#include "PacketHelper.h"
#include "messages/MqttSNRegister.h"
#include "messages/MqttSNPublish.h"

namespace mqttsn {

inet::Packet* PacketHelper::getRegisterPacket(uint16_t msgId, std::string topicName)
{
    const auto& payload = inet::makeShared<MqttSNRegister>();
    payload->setMsgType(MsgType::REGISTER);
    payload->setMsgId(msgId);
    payload->setTopicName(topicName);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("RegisterPacket");
    packet->insertAtBack(payload);

    return packet;
}

inet::Packet* PacketHelper::getPublishPacket(bool dupFlag, QoS qosFlag, bool retainFlag, TopicIdType topicIdTypeFlag, uint16_t topicId, uint16_t msgId, std::string data)
{
    const auto& payload = inet::makeShared<MqttSNPublish>();
    payload->setMsgType(MsgType::PUBLISH);
    payload->setDupFlag(dupFlag);
    payload->setQoSFlag(qosFlag);
    payload->setRetainFlag(retainFlag);
    payload->setTopicIdTypeFlag(topicIdTypeFlag);
    payload->setTopicId(topicId);
    payload->setMsgId(msgId);
    payload->setData(data);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("PublishPacket");
    packet->insertAtBack(payload);

    return packet;
}

} /* namespace mqttsn */
