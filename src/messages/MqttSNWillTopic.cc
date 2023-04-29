#include "MqttSNWillTopic.h"
#include "types/Length.h"
#include "types/Flag.h"

namespace mqttsn {

MqttSNWillTopic::MqttSNWillTopic()
{
    setLength(Length::WILLTOPIC_OCTETS, 0);
}

void MqttSNWillTopic::setQoSFlag(QoS qosFlag)
{
    flags = (flags & ~(0b11 << Flag::QUALITY_OF_SERVICE)) | (qosFlag << Flag::QUALITY_OF_SERVICE);
}

uint8_t MqttSNWillTopic::getQoSFlag()
{
    return (flags >> Flag::QUALITY_OF_SERVICE) & 0b11;
}

void MqttSNWillTopic::setRetainFlag(bool retainFlag)
{
    flags = (flags & ~(1 << Flag::RETAIN)) | (retainFlag << Flag::RETAIN);
}

bool MqttSNWillTopic::getRetainFlag()
{
    return (flags & (1 << Flag::RETAIN)) != 0;
}

void MqttSNWillTopic::setWillTopic(std::string topicName)
{
    uint16_t length = topicName.length();

    if (length <= getAvailableLength())
        willTopic = topicName.substr(0, length);
    else
        throw omnetpp::cRuntimeError("Will Topic name too long");

    setLength(Length::WILLTOPIC_OCTETS, length);
}

std::string MqttSNWillTopic::getWillTopic()
{
    return willTopic;
}

} /* namespace mqttsn */
