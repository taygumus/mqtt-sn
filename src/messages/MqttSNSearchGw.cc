#include "MqttSNSearchGw.h"

namespace mqttsn {

void MqttSNSearchGw::setRadius(uint8_t hops)
{
    radius = hops;
}

uint8_t MqttSNSearchGw::getRadius()
{
    return radius;
}

} /* namespace mqttsn */
