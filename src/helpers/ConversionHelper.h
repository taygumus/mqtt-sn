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

#ifndef HELPERS_CONVERSIONHELPER_H_
#define HELPERS_CONVERSIONHELPER_H_

#include "BaseHelper.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"

namespace mqttsn {

class ConversionHelper : public BaseHelper
{
    public:
        static QoS intToQoS(int value);
        static int qosToInt(QoS value);
        static TopicIdType stringToTopicIdType(const std::string& idType);
        static std::string topicIdTypeToString(TopicIdType idType);
};

} /* namespace mqttsn */

#endif /* HELPERS_CONVERSIONHELPER_H_ */
