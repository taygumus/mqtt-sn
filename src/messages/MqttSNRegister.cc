#include "MqttSNRegister.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNRegister::MqttSNRegister()
{
    setLength(Length::REGISTER_OCTETS, 0);
}

void MqttSNRegister::setTopicId(uint16_t id)
{
    topicId = id;
}

uint16_t MqttSNRegister::getTopicId()
{
    return topicId;
}

void MqttSNRegister::setMsgId(uint16_t messageId)
{
    msgId = messageId;
}

uint16_t MqttSNRegister::getMsgId()
{
    return msgId;
}

void MqttSNRegister::setTopicName(std::string name)
{
    uint16_t length = name.length();

    if (length <= getAvailableLength())
        topicName = name.substr(0, length);
    else
        throw omnetpp::cRuntimeError("Topic Name too long");

    setLength(Length::REGISTER_OCTETS, length);
}

std::string MqttSNRegister::getTopicName()
{
    return topicName;
}

} /* namespace mqttsn */
