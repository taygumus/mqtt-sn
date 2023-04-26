#include "MqttSNConnect.h"
#include "types/Flags.h"

namespace mqttsn {

void MqttSNConnect::setWillFlag(bool willFlag)
{
    flags |= (willFlag << Flags::WILL);
}

bool MqttSNConnect::getWillFlag()
{
    return (flags & (1 << Flags::WILL)) != 0;
}

void MqttSNConnect::setCleanSessionFlag(bool cleanSessionFlag)
{
    flags |= (cleanSessionFlag << Flags::CLEAN_SESSION);
}

bool MqttSNConnect::getCleanSessionFlag()
{
    return (flags & (1 << Flags::CLEAN_SESSION)) != 0;
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
