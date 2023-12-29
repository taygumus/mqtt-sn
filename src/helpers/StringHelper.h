#ifndef HELPERS_STRINGHELPER_H_
#define HELPERS_STRINGHELPER_H_

#include "BaseHelper.h"

namespace mqttsn {

class StringHelper : public BaseHelper
{
    private:
        static const std::string base64_chars;

    public:
        static std::string base64Encode(const std::string& inputString);
        static std::string base64Decode(const std::string& inputString);
        static std::string appendCounterToString(const std::string& inputString, int counter);
        static std::string sanitizeSpaces(const std::string& inputString);
};

} /* namespace mqttsn */

#endif /* HELPERS_STRINGHELPER_H_ */
