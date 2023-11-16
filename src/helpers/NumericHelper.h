#ifndef HELPERS_NUMERICHELPER_H_
#define HELPERS_NUMERICHELPER_H_

#include "BaseHelper.h"

namespace mqttsn {

class NumericHelper : public BaseHelper
{
    public:
        static void incrementCounter(int* counter);
};

} /* namespace mqttsn */

#endif /* HELPERS_NUMERICHELPER_H_ */
