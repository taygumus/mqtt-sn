#include "MqttSNSearchGw.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNSearchGw::MqttSNSearchGw()
{
    setLength(Length::SEARCHGW_OCTETS, 0);
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
