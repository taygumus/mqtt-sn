#include "MqttSNConnect.h"
#include "types/Flag.h"

namespace mqttsn {

void MqttSNConnect::setWillFlag(bool willFlag)
{
    flags = (flags & ~(1 << Flag::WILL)) | (willFlag << Flag::WILL);
}

bool MqttSNConnect::getWillFlag()
{
    return (flags & (1 << Flag::WILL)) != 0;
}

void MqttSNConnect::setCleanSessionFlag(bool cleanSessionFlag)
{
    flags = (flags & ~(1 << Flag::CLEAN_SESSION)) | (cleanSessionFlag << Flag::CLEAN_SESSION);
}

bool MqttSNConnect::getCleanSessionFlag()
{
    return (flags & (1 << Flag::CLEAN_SESSION)) != 0;
}

uint8_t MqttSNConnect::getProtocolId()
{
    return protocolId;
}

void MqttSNConnect::setDuration(uint16_t seconds)
{
    duration = seconds;
}

uint16_t MqttSNConnect::getDuration()
{
    return duration;
}

void MqttSNConnect::setClientId(std::string id) {
    if (id.length() <= 23)
        clientId = id;
    else
        clientId = id.substr(0, 23);
}

std::string MqttSNConnect::getClientId() {
    return clientId;
}


} /* namespace mqttsn */
