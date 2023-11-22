#include "ConversionHelper.h"

namespace mqttsn {

QoS ConversionHelper::intToQos(int value)
{
    // convert an integer to QoS enumeration
    switch (value) {
        case -1:
            return QOS_MINUS_ONE;

        case 0:
            return QOS_ZERO;

        case 1:
            return QOS_ONE;

        case 2:
            return QOS_TWO;

        default:
            throw omnetpp::cRuntimeError("Invalid QoS value: %d", value);
    }
}

int ConversionHelper::qosToInt(QoS value)
{
    // convert a QoS enumeration to an integer
    switch (value) {
        case QOS_MINUS_ONE:
            return -1;

        case QOS_ZERO:
            return 0;

        case QOS_ONE:
            return 1;

        case QOS_TWO:
            return 2;

        default:
            throw omnetpp::cRuntimeError("Invalid QoS value");
    }
}

} /* namespace mqttsn */
