#include "MqttSNSearchGw.h"

namespace mqttsn {

MqttSNSearchGw::MqttSNSearchGw()
{
    //
}

void MqttSNSearchGw::setRadius(uint8_t hops)
{
    radius = hops;
}

uint8_t MqttSNSearchGw::getRadius()
{
    return radius;
}

} /* namespace mqttsn */
