#include "MqttSNDisconnect.h"
#include "types/Length.h"

namespace mqttsn {

void MqttSNDisconnect::setDuration(uint16_t seconds)
{
    uint32_t field = duration;
    MqttSNBase::setOptionalField(seconds, Length::TWO_OCTETS, field);
    duration = field;
}

uint16_t MqttSNDisconnect::getDuration() const
{
    return duration;
}

} /* namespace mqttsn */
