#include "MqttSNMessage.h"

namespace mqttsn {

void MqttSNMessage::setLength(unsigned __int16 length)
{
    //
    EV << length;
}

unsigned __int16 MqttSNMessage::getLength()
{
    //
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
