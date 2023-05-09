#include "MqttSNBaseWithWillMsg.h"
#include "types/Length.h"

namespace mqttsn {

void MqttSNBaseWithWillMsg::setWillMsg(std::string willMessage)
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
