#include "MqttSNMessage.h"
#include "types/Length.h"

namespace mqttsn {

void MqttSNMessage::setLength(uint16_t fixedLength, uint16_t variableLength)
{
    length.clear();
    uint16_t minLength = fixedLength + variableLength + Length::TWO_OCTETS;

    if (minLength <= UINT8_MAX) {
        length.push_back(static_cast<uint8_t>(minLength));
    }
    else {
        minLength += Length::TWO_OCTETS;
        length.push_back(0x01);
        length.push_back(static_cast<uint8_t>(minLength & 0xFF));
        length.push_back(static_cast<uint8_t>((minLength >> 8) & 0xFF));
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

uint16_t MqttSNMessage::getAvailableLength()
{
    return UINT16_MAX - getLength();
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
