#include "MqttSNBaseWithWillTopic.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNBaseWithWillTopic::MqttSNBaseWithWillTopic()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNBaseWithWillTopic::setQoSFlag(QoS qosFlag)
{
    MqttSNBase::setFlag(qosFlag, Flag::QUALITY_OF_SERVICE, flags);
}

uint8_t MqttSNBaseWithWillTopic::getQoSFlag() const
{
    return MqttSNBase::getFlag(Flag::QUALITY_OF_SERVICE, flags);
}

void MqttSNBaseWithWillTopic::setRetainFlag(bool retainFlag)
{
    MqttSNBase::setBooleanFlag(retainFlag, Flag::RETAIN, flags);
}

bool MqttSNBaseWithWillTopic::getRetainFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::RETAIN, flags);
}

void MqttSNBaseWithWillTopic::setWillTopic(std::string topicName)
{
    MqttSNBase::setStringField(topicName, "Will topic name too long", willTopic);
}

std::string MqttSNBaseWithWillTopic::getWillTopic() const
{
    return willTopic;
}

} /* namespace mqttsn */
