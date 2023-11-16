#include "NumericHelper.h"

namespace mqttsn {

void NumericHelper::incrementCounter(int* counter) {
    if (*counter == std::numeric_limits<int>::max()) {
        *counter = 0;
    }
    else {
        (*counter)++;
    }
}

} /* namespace mqttsn */
