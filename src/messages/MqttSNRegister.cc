#include "MqttSNRegister.h"
#include "types/shared/Length.h"

namespace mqttsn {

void MqttSNRegister::setTopicName(const std::string& name)
{
    MqttSNBase::setStringField(
            name,
            Length::ZERO_OCTETS,
            MqttSNBase::getAvailableLength(),
            "Topic name length out of range",
            topicName
    );
}

std::string MqttSNRegister::getTopicName() const
{
    return topicName;
}

} /* namespace mqttsn */
