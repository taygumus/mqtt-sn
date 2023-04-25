#include "MqttSNMessage.h"

namespace mqttsn {

void MqttSNMessage::setLength(uint16_t messageLength)
{
    if (messageLength < 256) {
        length.push_back(static_cast<uint8_t>(messageLength));
    }
    else {
        length.push_back(0x01);
        length.push_back(static_cast<uint8_t>(messageLength & 0xFF));
        length.push_back(static_cast<uint8_t>((messageLength >> 8) & 0xFF));
    }
}

uint16_t MqttSNMessage::getLength()
{
    if (length.size() == 1) {
        return static_cast<uint16_t>(length[0]);
    }

    if (length.size() == 3 && length[0] == 0x01) {
        return static_cast<uint16_t>(length[2]) << 8 | static_cast<uint16_t>(length[1]);
    }

    return 0;
}

void MqttSNMessage::setMsgType(MsgType messageType)
{
    msgType = messageType;
}

MsgType MqttSNMessage::getMsgType()
{
    return msgType;
}

} /* namespace mqttsn */
