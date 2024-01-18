#include "MqttSNSubscribe.h"

namespace mqttsn {

void MqttSNSubscribe::setDupFlag(bool dupFlag)
{
    MqttSNBase::setBooleanFlag(dupFlag, Flag::DUP, flags);
}

bool MqttSNSubscribe::getDupFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::DUP, flags);
}

void MqttSNSubscribe::setQoSFlag(QoS qosFlag)
{
    MqttSNBase::setFlag(qosFlag, Flag::QUALITY_OF_SERVICE, flags);
}

uint8_t MqttSNSubscribe::getQoSFlag() const
{
    return MqttSNBase::getFlag(Flag::QUALITY_OF_SERVICE, flags);
}

} /* namespace mqttsn */
