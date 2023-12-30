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
        static std::string sanitizeSpaces(const std::string& inputString);
        static std::string appendCounterToString(const std::string& inputString, const std::string& delimiter, int counter);
        static std::string getStringBeforeDelimiter(const std::string& inputString, const std::string& delimiter);
};

} /* namespace mqttsn */

#endif /* HELPERS_STRINGHELPER_H_ */
