#include "MqttSNPingReq.h"
#include "types/shared/Length.h"

namespace mqttsn {

void MqttSNPingReq::setClientId(const std::string& id)
{
    MqttSNBase::setStringField(
            id,
            Length::ONE_OCTET,
            Length::CLIENT_ID_OCTETS,
            "Client ID length out of range",
            clientId
    );
}

std::string MqttSNPingReq::getClientId() const
{
    return clientId;
}

} /* namespace mqttsn */
