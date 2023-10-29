#ifndef HELPERS_STRINGHELPER_H_
#define HELPERS_STRINGHELPER_H_

#include <iostream>

namespace mqttsn {

class StringHelper
{
    public:
        static std::string appendCounterToString(std::string inputString, int counter);
        static std::string base64Encode(std::string inputString);
        static std::string sanitizeSpaces(std::string inputString);
};

} /* namespace mqttsn */

#endif /* HELPERS_STRINGHELPER_H_ */
