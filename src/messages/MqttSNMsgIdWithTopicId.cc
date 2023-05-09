#include "MqttSNMsgIdWithTopicId.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNMsgIdWithTopicId::MqttSNMsgIdWithTopicId()
{
    addLength(Length::TWO_OCTETS);
}

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

} /* namespace mqttsn */
