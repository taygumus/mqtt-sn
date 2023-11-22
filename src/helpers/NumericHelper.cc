#include "NumericHelper.h"

namespace mqttsn {

void NumericHelper::incrementCounter(int* counter)
{
    if (*counter == std::numeric_limits<int>::max()) {
        *counter = 0;
    }
    else {
        (*counter)++;
    }
}

QoS NumericHelper::minQos(QoS first, QoS second)
{
    // calculate the minimum QoS value
    int minQosValue = std::min(
            ConversionHelper::qosToInt(first),
            ConversionHelper::qosToInt(second)
    );

    // convert the minimum QoS value back to QoS enumeration
    return ConversionHelper::intToQos(minQosValue);
}

} /* namespace mqttsn */
