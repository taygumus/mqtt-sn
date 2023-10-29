#include "ConversionHelper.h"

namespace mqttsn {

QoS ConversionHelper::intToQoS(int value)
{
    // convert an integer to QoS enumeration
    switch (value) {
        case 0:
            return QOS_ZERO;

        case 1:
            return QOS_ONE;

        case 2:
            return QOS_TWO;

        case -1:
            return QOS_MINUS_ONE;

        default:
            throw omnetpp::cRuntimeError("Invalid QoS value: %d", value);
    }
}

} /* namespace mqttsn */
