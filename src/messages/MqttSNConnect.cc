#include "MqttSNConnect.h"
#include "types/Length.h"
#include "types/Flag.h"

namespace mqttsn {

MqttSNConnect::MqttSNConnect()
{
    MqttSNBase::addLength(Length::TWO_OCTETS);
}

void MqttSNConnect::setWillFlag(bool willFlag)
{
    MqttSNBase::setBooleanFlag(willFlag, Flag::WILL, flags);
}

bool MqttSNConnect::getWillFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::WILL, flags);
}

void MqttSNConnect::setCleanSessionFlag(bool cleanSessionFlag)
{
    MqttSNBase::setBooleanFlag(cleanSessionFlag, Flag::CLEAN_SESSION, flags);
}

bool MqttSNConnect::getCleanSessionFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::CLEAN_SESSION, flags);
}

uint8_t MqttSNConnect::getProtocolId() const
{
    return protocolId;
}

void MqttSNConnect::setClientId(std::string id)
{
    MqttSNBase::setClientId(id, clientId);
}

std::string MqttSNConnect::getClientId() const
{
    return clientId;
}

} /* namespace mqttsn */
