#include "MqttSNUnsubscribe.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNUnsubscribe::MqttSNUnsubscribe()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNUnsubscribe::setTopicIdTypeFlag(TopicIdType topicIdTypeFlag)
{
    MqttSNBase::setFlag(topicIdTypeFlag, Flag::TOPIC_ID_TYPE, flags);
}

uint8_t MqttSNUnsubscribe::getTopicIdTypeFlag() const
{
    return MqttSNBase::getFlag(Flag::TOPIC_ID_TYPE, flags);
}

void MqttSNUnsubscribe::setTopicName(const std::string& name)
{
    uint8_t topicIdFlag = getTopicIdTypeFlag();

    if (topicIdFlag == TopicIdType::NORMAL_TOPIC)
        MqttSNBase::setStringField(
                name,
                Length::ZERO_OCTETS,
                MqttSNBase::getAvailableLength(),
                "Topic name length out of range",
                topicName
        );
    else if (topicIdFlag == TopicIdType::SHORT_TOPIC_NAME)
        MqttSNBase::setStringField(
                name,
                Length::ZERO_OCTETS,
                Length::TWO_OCTETS,
                "Short topic name length out of range",
                topicName
        );
    else
        throw omnetpp::cRuntimeError("The topic ID type flag is not correctly set to either topic name or short topic name");
}

std::string MqttSNUnsubscribe::getTopicName() const
{
    return topicName;
}

void MqttSNUnsubscribe::setTopicId(uint16_t id)
{
    if (id == UINT16_MAX)
        throw omnetpp::cRuntimeError("Reserved topic ID");

    if (getTopicIdTypeFlag() == TopicIdType::PRE_DEFINED_TOPIC_ID) {
        uint32_t field = topicId;
        MqttSNBase::setOptionalField(id, Length::TWO_OCTETS, field);
        topicId = field;
    }
    else {
        throw omnetpp::cRuntimeError("The topic ID type flag is not correctly set to pre-defined topic ID");
    }
}

uint16_t MqttSNUnsubscribe::getTopicId() const
{
    return topicId;
}

} /* namespace mqttsn */
