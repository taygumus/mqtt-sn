#include "StringHelper.h"
#include <algorithm>

namespace mqttsn {

const std::string StringHelper::base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string StringHelper::base64Encode(const std::string& inputString)
{
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

std::string StringHelper::base64Decode(const std::string& inputString)
{
    int val = 0, valb = -8;
    std::string decodedString;

    for (unsigned char c : inputString) {
        if (isalnum(c) || c == '+' || c == '/') {
            val = (val << 6) + base64_chars.find(c);
            valb += 6;

            if (valb >= 0) {
                decodedString.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        else if (c != '=') {
            throw std::runtime_error("Invalid character in Base64 string");
        }
    }

    return decodedString;
}

std::string StringHelper::sanitizeSpaces(const std::string& inputString)
{
    std::string sanitizedString = inputString;
    sanitizedString.erase(std::remove_if(sanitizedString.begin(), sanitizedString.end(), ::isspace), sanitizedString.end());

    return sanitizedString;
}

std::string StringHelper::appendCounterToString(const std::string& inputString, const std::string& delimiter, int counter)
{
    if (counter == 0) {
        return inputString;
    }

    return inputString + delimiter + std::to_string(counter);
}

std::string StringHelper::getStringBeforeDelimiter(const std::string& inputString, const std::string& delimiter)
{
    size_t delimiterPos = inputString.find(delimiter);

    if (delimiterPos != std::string::npos) {
        // if the delimiter is found, return the substring before the delimiter
        return inputString.substr(0, delimiterPos);
    }
    else {
        // if the delimiter is not found, return the original string
        return inputString;
    }
}

} /* namespace mqttsn */
