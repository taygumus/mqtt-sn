#ifndef HELPERS_CONVERSIONHELPER_H_
#define HELPERS_CONVERSIONHELPER_H_

#include "BaseHelper.h"
#include "types/shared/QoS.h"

namespace mqttsn {

class ConversionHelper : public BaseHelper
{
    public:
        static QoS intToQoS(int value);
};

} /* namespace mqttsn */

#endif /* HELPERS_CONVERSIONHELPER_H_ */
