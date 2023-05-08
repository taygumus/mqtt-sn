#include "MqttSNDisconnect.h"
#include "types/Length.h"

namespace mqttsn {

void MqttSNDisconnect::setDuration(uint16_t seconds)
{
    if (seconds == duration)
        return;

    uint16_t length = seconds == 0 ? Length::ZERO_OCTETS : Length::TWO_OCTETS;
    uint16_t prevLength = duration == 0 ? Length::ZERO_OCTETS : Length::TWO_OCTETS;

    duration = seconds;

    if (length == prevLength)
        return;

    addLength(length, prevLength);
}

uint16_t MqttSNDisconnect::getDuration() const
{
    return duration;
}

} /* namespace mqttsn */
