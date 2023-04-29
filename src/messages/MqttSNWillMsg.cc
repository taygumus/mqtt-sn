#include "MqttSNWillMsg.h"

namespace mqttsn {

void MqttSNWillMsg::setWillMsg(std::string willMessage)
{
    uint16_t length = willMessage.length();

    if (length <= getAvailableLength())
        willMsg = willMessage.substr(0, length);
    else
        throw omnetpp::cRuntimeError("Will Message too long");

    setLength(0, length);
}

std::string MqttSNWillMsg::getWillMsg() const {
    return willMsg;
}

} /* namespace mqttsn */
