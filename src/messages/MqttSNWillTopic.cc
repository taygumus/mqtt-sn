#include "MqttSNWillTopic.h"
#include "types/Flag.h"

namespace mqttsn {

void MqttSNWillTopic::setQoSFlag(QoS qosFlag)
{
    flags = (flags & ~(0b11 << Flag::QUALITY_OF_SERVICE)) | (qosFlag << Flag::QUALITY_OF_SERVICE);
}

uint8_t MqttSNWillTopic::getQoSFlag()
{
    return (flags >> Flag::QUALITY_OF_SERVICE) & 0b11;
}

void MqttSNWillTopic::setRetainFlag(bool retainFlag)
{
    flags = (flags & ~(1 << Flag::RETAIN)) | (retainFlag << Flag::RETAIN);
}

bool MqttSNWillTopic::getRetainFlag()
{
    return (flags & (1 << Flag::RETAIN)) != 0;
}

void MqttSNWillTopic::setWillTopic(std::string topicName) {
    uint16_t left = 0;
    uint16_t length = getLength();

    if (length > 3 && length < 256) {
        left = length - 3;
    }
    else if (length >= 256) {
        left = length - 5;
    }
    else {
        throw omnetpp::cRuntimeError("WillTopic name cannot be set");
    }

    uint16_t strLength = topicName.length();

    if (strLength <= left) {
        willTopic = topicName.substr(0, strLength);
    }
    else {
        throw omnetpp::cRuntimeError("WillTopic name too long");
    }
}

std::string MqttSNWillTopic::getWillTopic() {
    return willTopic;
}

} /* namespace mqttsn */
