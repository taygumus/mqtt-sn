#include "MqttSNAdvertise.h"
#include "types/shared/Length.h"

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
