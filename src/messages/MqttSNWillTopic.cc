#include "MqttSNWillTopic.h"
#include "types/Flag.h"

namespace mqttsn {

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

} /* namespace mqttsn */
