//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

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
