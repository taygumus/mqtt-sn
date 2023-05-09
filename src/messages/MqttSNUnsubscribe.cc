#include "MqttSNUnsubscribe.h"
#include "types/Length.h"
#include "types/TopicIdType.h"

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

void MqttSNUnsubscribe::setTopicName(std::string name)
{
    /*
    TopicIdType topicIdFlag = getTopicIdTypeFlag();

    if (topicIdFlag == TopicIdType::TOPIC_NAME) {
        MqttSNBase::setStringField(name, "Topic name too long", topicName);
    }
    else if (topicIdFlag == TopicIdType::SHORT_TOPIC_NAME) {

    }
    */
}

std::string MqttSNUnsubscribe::getTopicName() const
{
    return topicName;
}

/*
void MqttSNMsgIdWithTopicId::setTopicId(uint16_t id)
{
    if (id == 0 || id == UINT16_MAX)
        throw omnetpp::cRuntimeError("Reserved topic ID");

    topicId = id;
}

uint16_t MqttSNMsgIdWithTopicId::getTopicId() const
{
    return topicId;
}
*/

} /* namespace mqttsn */
