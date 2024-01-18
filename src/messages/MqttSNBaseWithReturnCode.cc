#include "MqttSNBaseWithReturnCode.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNBaseWithReturnCode::MqttSNBaseWithReturnCode()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNBaseWithReturnCode::setReturnCode(ReturnCode code)
{
    returnCode = code;
}

ReturnCode MqttSNBaseWithReturnCode::getReturnCode() const
{
    return returnCode;
}

} /* namespace mqttsn */
