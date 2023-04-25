#include "MqttSNGwInfo.h"

namespace mqttsn {

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
