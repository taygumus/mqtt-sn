#include "MqttSNAdvertise.h"

namespace mqttsn {

void MqttSNAdvertise::setGwId(uint8_t gatewayId)
{
    gwId = gatewayId;
}

uint8_t MqttSNAdvertise::getGwId()
{
    return gwId;
}

void MqttSNAdvertise::setDuration(uint16_t seconds)
{
    duration = seconds;
}

uint16_t MqttSNAdvertise::getDuration()
{
    return duration;
}


} /* namespace mqttsn */
