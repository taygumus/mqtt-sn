#include "MqttSNConnAck.h"

namespace mqttsn {

void MqttSNConnAck::setReturnCode(ReturnCode code)
{
    returnCode = code;
}

ReturnCode MqttSNConnAck::getReturnCode()
{
    return returnCode;
}

} /* namespace mqttsn */
