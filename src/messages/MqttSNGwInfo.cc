#include "MqttSNGwInfo.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNGwInfo::MqttSNGwInfo()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNGwInfo::setGwId(uint8_t gatewayId)
{
    gwId = gatewayId;
}

uint8_t MqttSNGwInfo::getGwId() const
{
    return gwId;
}

void MqttSNGwInfo::setGwAdd(uint32_t gatewayAddress)
{
    MqttSNBase::setOptionalField(gatewayAddress, Length::FOUR_OCTETS, gwAdd);
}

uint32_t MqttSNGwInfo::getGwAdd() const
{
    return gwAdd;
}

} /* namespace mqttsn */
