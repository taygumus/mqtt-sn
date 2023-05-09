#include "MqttSNMsgIdWithTopicIdExtended.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNMsgIdWithTopicIdExtended::MqttSNMsgIdWithTopicIdExtended()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNMsgIdWithTopicIdExtended::setReturnCode(ReturnCode code)
{
    returnCode = code;
}

ReturnCode MqttSNMsgIdWithTopicIdExtended::getReturnCode() const
{
    return returnCode;
}

} /* namespace mqttsn */
