#include "MqttSNSearchGw.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNSearchGw::MqttSNSearchGw()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNSearchGw::setRadius(uint8_t hops)
{
    radius = hops;
}

uint8_t MqttSNSearchGw::getRadius() const
{
    return radius;
}

} /* namespace mqttsn */
