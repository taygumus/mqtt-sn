#include "MqttSNMsgIdWithTopicId.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNMsgIdWithTopicId::MqttSNMsgIdWithTopicId()
{
    MqttSNBase::addLength(Length::TWO_OCTETS);
}

void MqttSNMsgIdWithTopicId::setTopicId(uint16_t id)
{
    if (id == UINT16_MAX)
        throw omnetpp::cRuntimeError("Reserved topic ID");

    topicId = id;
}

uint16_t MqttSNMsgIdWithTopicId::getTopicId() const
{
    return topicId;
}

} /* namespace mqttsn */
