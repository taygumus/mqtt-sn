#include "MqttSNMsgIdWithTopicIdPlus.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNMsgIdWithTopicIdPlus::MqttSNMsgIdWithTopicIdPlus()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNMsgIdWithTopicIdPlus::setReturnCode(ReturnCode code)
{
    returnCode = code;
}

ReturnCode MqttSNMsgIdWithTopicIdPlus::getReturnCode() const
{
    return returnCode;
}

} /* namespace mqttsn */
