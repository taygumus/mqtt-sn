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

#include "ConversionHelper.h"

namespace mqttsn {

QoS ConversionHelper::intToQoS(int value)
{
    // convert an integer to QoS enumeration
    switch (value) {
        case -1:
            return QOS_MINUS_ONE;

        case 0:
            return QOS_ZERO;

        case 1:
            return QOS_ONE;

        case 2:
            return QOS_TWO;

        default:
            throw omnetpp::cRuntimeError("Invalid QoS value: %d", value);
    }
}

int ConversionHelper::qosToInt(QoS value)
{
    // convert a QoS enumeration to an integer
    switch (value) {
        case QOS_MINUS_ONE:
            return -1;

        case QOS_ZERO:
            return 0;

        case QOS_ONE:
            return 1;

        case QOS_TWO:
            return 2;

        default:
            throw omnetpp::cRuntimeError("Invalid QoS value");
    }
}

TopicIdType ConversionHelper::stringToTopicIdType(const std::string& idType)
{
    // convert from a string identifier to a topic ID type enumeration
    if (idType == "normal") {
        return TopicIdType::NORMAL_TOPIC_ID;
    }
    else if (idType == "predefined") {
        return TopicIdType::PRE_DEFINED_TOPIC_ID;
    }
    else if (idType == "short") {
        return TopicIdType::SHORT_TOPIC_ID;
    }

    throw omnetpp::cRuntimeError("Invalid topic ID type");
}

std::string ConversionHelper::topicIdTypeToString(TopicIdType idType)
{
    // convert a topic ID type enumeration to its corresponding string identifier
    switch (idType) {
        case TopicIdType::NORMAL_TOPIC_ID:
            return "normal";

        case TopicIdType::PRE_DEFINED_TOPIC_ID:
            return "predefined";

        case TopicIdType::SHORT_TOPIC_ID:
            return "short";

        default:
            throw omnetpp::cRuntimeError("Invalid topic ID type");
    }
}

} /* namespace mqttsn */
