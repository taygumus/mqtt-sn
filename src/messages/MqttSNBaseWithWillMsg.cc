#include "MqttSNBaseWithWillMsg.h"

namespace mqttsn {

void MqttSNBaseWithWillMsg::setWillMsg(std::string willMessage)
{
    MqttSNBase::setStringField(willMessage, "Will message too long", willMsg);
}

std::string MqttSNBaseWithWillMsg::getWillMsg() const
{
    return willMsg;
}

} /* namespace mqttsn */
