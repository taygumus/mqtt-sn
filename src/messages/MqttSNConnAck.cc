#include "MqttSNConnAck.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNConnAck::MqttSNConnAck()
{
    setLength(Length::CONNACK_OCTETS, 0);
}

void MqttSNConnAck::setReturnCode(ReturnCode code)
{
    returnCode = code;
}

ReturnCode MqttSNConnAck::getReturnCode()
{
    return returnCode;
}

} /* namespace mqttsn */
