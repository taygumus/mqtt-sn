#include "MqttSNRegAck.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNRegAck::MqttSNRegAck()
{
    setLength(Length::REGACK_OCTETS, 0);
}

void MqttSNRegAck::setTopicId(uint16_t id)
{
    topicId = id;
}

uint16_t MqttSNRegAck::getTopicId()
{
    return topicId;
}

void MqttSNRegAck::setMsgId(uint16_t messageId)
{
    msgId = messageId;
}

uint16_t MqttSNRegAck::getMsgId()
{
    return msgId;
}

void MqttSNRegAck::setReturnCode(ReturnCode code)
{
    returnCode = code;
}

ReturnCode MqttSNRegAck::getReturnCode()
{
    return returnCode;
}

} /* namespace mqttsn */
