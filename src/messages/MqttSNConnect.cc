#include "MqttSNConnect.h"
#include "types/Length.h"
#include "types/Flag.h"

namespace mqttsn {

MqttSNConnect::MqttSNConnect()
{
    setLength(Length::CONNECT_OCTETS, 0);
}

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

void MqttSNConnect::setClientId(std::string id)
{
    uint16_t length = id.length();

    if (length >= Length::ONE_OCTET && length <= Length::CLIENT_ID_OCTETS)
        clientId = id.substr(0, length);
    else
        throw omnetpp::cRuntimeError("Client Id length must be between 1 and 23");

    setLength(Length::CONNECT_OCTETS, length);
}

std::string MqttSNConnect::getClientId()
{
    return clientId;
}

} /* namespace mqttsn */
