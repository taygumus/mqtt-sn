#include "MqttSNGwInfo.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNGwInfo::MqttSNGwInfo()
{
    setLength(Length::GWINFO_OCTETS, 0);
}

void MqttSNGwInfo::setGwId(uint8_t gatewayId)
{
    gwId = gatewayId;
}

uint8_t MqttSNGwInfo::getGwId()
{
    return gwId;
}

void MqttSNGwInfo::setGwAdd(uint32_t gatewayAddress)
{
    gwAdd = gatewayAddress;
}

uint32_t MqttSNGwInfo::getGwAdd()
{
    return gwAdd;
}



} /* namespace mqttsn */
