#include "MqttSNBaseWithWillMsg.h"

namespace mqttsn {

void MqttSNBaseWithWillMsg::setWillMsg(std::string willMessage)
{
    uint16_t length = willMessage.length();
    uint16_t prevLength;

    if (length <= getAvailableLength()) {
        prevLength = willMsg.size();
        willMsg = willMessage;
    }
    else {
        throw omnetpp::cRuntimeError("Will message too long");
    }

    addLength(length, prevLength);
}

std::string MqttSNBaseWithWillMsg::getWillMsg() const
{
    return willMsg;
}

} /* namespace mqttsn */
