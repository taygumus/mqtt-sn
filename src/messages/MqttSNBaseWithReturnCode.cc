#include "MqttSNBaseWithReturnCode.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNBaseWithReturnCode::MqttSNBaseWithReturnCode()
{
    addLength(Length::ONE_OCTET);
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
