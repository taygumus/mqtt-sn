#include "MqttSNSubAck.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNSubAck::MqttSNSubAck()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNSubAck::setQoSFlag(QoS qosFlag)
{
    MqttSNBase::setFlag(qosFlag, Flag::QUALITY_OF_SERVICE, flags);
}

uint8_t MqttSNSubAck::getQoSFlag() const
{
    return MqttSNBase::getFlag(Flag::QUALITY_OF_SERVICE, flags);
}

} /* namespace mqttsn */
