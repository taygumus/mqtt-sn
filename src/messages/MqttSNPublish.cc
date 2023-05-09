#include "MqttSNPublish.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNPublish::MqttSNPublish()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNPublish::setDupFlag(bool dupFlag)
{
    MqttSNBase::setBooleanFlag(dupFlag, Flag::DUP, flags);
}

bool MqttSNPublish::getDupFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::DUP, flags);
}

void MqttSNPublish::setQoSFlag(QoS qosFlag)
{
    MqttSNBase::setFlag(qosFlag, Flag::QUALITY_OF_SERVICE, flags);
}

uint8_t MqttSNPublish::getQoSFlag() const
{
    return MqttSNBase::getFlag(Flag::QUALITY_OF_SERVICE, flags);
}

void MqttSNPublish::setRetainFlag(bool retainFlag)
{
    MqttSNBase::setBooleanFlag(retainFlag, Flag::RETAIN, flags);
}

bool MqttSNPublish::getRetainFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::RETAIN, flags);
}

void MqttSNPublish::setTopicIdTypeFlag(TopicIdType topicIdTypeFlag)
{
    MqttSNBase::setFlag(topicIdTypeFlag, Flag::TOPIC_ID_TYPE, flags);
}

uint8_t MqttSNPublish::getTopicIdTypeFlag() const
{
    return MqttSNBase::getFlag(Flag::TOPIC_ID_TYPE, flags);
}

void MqttSNPublish::setData(std::string stringData)
{
    MqttSNBase::setStringField(
            stringData,
            Length::ZERO_OCTETS,
            MqttSNBase::getAvailableLength(),
            "Data string length out of range",
            data
    );
}

std::string MqttSNPublish::getData() const
{
    return data;
}

} /* namespace mqttsn */
