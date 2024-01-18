#include "MqttSNBaseWithWillMsg.h"
#include "types/shared/Length.h"

namespace mqttsn {

void MqttSNBaseWithWillMsg::setWillMsg(const std::string& willMessage)
{
    MqttSNBase::setStringField(
            willMessage,
            Length::ZERO_OCTETS,
            MqttSNBase::getAvailableLength(),
            "Will message length out of range",
            willMsg
    );
}

std::string MqttSNBaseWithWillMsg::getWillMsg() const
{
    return willMsg;
}

} /* namespace mqttsn */
