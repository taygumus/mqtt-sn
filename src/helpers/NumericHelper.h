#ifndef HELPERS_NUMERICHELPER_H_
#define HELPERS_NUMERICHELPER_H_

#include "BaseHelper.h"
#include "ConversionHelper.h"
#include "types/shared/QoS.h"

namespace mqttsn {

class NumericHelper : public BaseHelper
{
    public:
        static void incrementCounter(int* counter);
        static QoS minQos(QoS first, QoS second);
};

} /* namespace mqttsn */

#endif /* HELPERS_NUMERICHELPER_H_ */
