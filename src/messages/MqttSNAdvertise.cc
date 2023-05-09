#include "MqttSNAdvertise.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNAdvertise::MqttSNAdvertise()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNAdvertise::setGwId(uint8_t gatewayId)
{
    gwId = gatewayId;
}

uint8_t MqttSNAdvertise::getGwId() const
{
    return gwId;
}

} /* namespace mqttsn */
