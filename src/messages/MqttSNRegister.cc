#include "MqttSNRegister.h"

namespace mqttsn {

void MqttSNRegister::setTopicName(std::string name)
{
    MqttSNBase::setStringField(name, "Topic name too long", topicName);
}

std::string MqttSNRegister::getTopicName() const
{
    return topicName;
}

} /* namespace mqttsn */
