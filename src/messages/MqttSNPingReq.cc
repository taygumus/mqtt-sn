#include "MqttSNPingReq.h"

namespace mqttsn {

void MqttSNPingReq::setClientId(std::string id)
{
    MqttSNBase::setClientId(id, clientId);
}

std::string MqttSNPingReq::getClientId() const
{
    return clientId;
}

} /* namespace mqttsn */
