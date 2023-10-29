#include "StringHelper.h"
#include <algorithm>

namespace mqttsn {

std::string StringHelper::appendCounterToString(std::string inputString, int counter)
{
    if (counter == 0) {
        return inputString;
    }

    return inputString + std::to_string(counter);
}

std::string StringHelper::base64Encode(std::string inputString)
{
    // encode the string to Base64
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int val = 0, valb = -6;
    std::string encodedString;

    for (unsigned char c : inputString) {
        val = (val << 8) + c;
        valb += 8;

        while (valb >= 0) {
            encodedString.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        encodedString.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (encodedString.size() % 4) {
        encodedString.push_back('=');
    }

    return encodedString;
}

std::string StringHelper::sanitizeSpaces(std::string inputString)
{
    std::string sanitizedString = inputString;
    sanitizedString.erase(std::remove_if(sanitizedString.begin(), sanitizedString.end(), ::isspace), sanitizedString.end());

    return sanitizedString;
}

} /* namespace mqttsn */
