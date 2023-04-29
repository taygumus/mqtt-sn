#include "MqttSNAdvertise.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNAdvertise::MqttSNAdvertise()
{
    setLength(Length::ADVERTISE_OCTETS, 0);
}

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
