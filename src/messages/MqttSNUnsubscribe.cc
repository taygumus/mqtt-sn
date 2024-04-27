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

#include "MqttSNUnsubscribe.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNUnsubscribe::MqttSNUnsubscribe()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNUnsubscribe::setTopicIdTypeFlag(TopicIdType topicIdTypeFlag)
{
    MqttSNBase::setFlag(topicIdTypeFlag, Flag::TOPIC_ID_TYPE, flags);
}

uint8_t MqttSNUnsubscribe::getTopicIdTypeFlag() const
{
    return MqttSNBase::getFlag(Flag::TOPIC_ID_TYPE, flags);
}

void MqttSNUnsubscribe::setTopicName(const std::string& name)
{
    uint8_t topicIdFlag = getTopicIdTypeFlag();

    if (topicIdFlag == TopicIdType::NORMAL_TOPIC_ID)
        MqttSNBase::setStringField(
                name,
                Length::ZERO_OCTETS,
                MqttSNBase::getAvailableLength(),
                "Topic name length out of range",
                topicName
        );
    else if (topicIdFlag == TopicIdType::SHORT_TOPIC_ID)
        MqttSNBase::setStringField(
                name,
                Length::ZERO_OCTETS,
                Length::TWO_OCTETS,
                "Short topic name length out of range",
                topicName
        );
    else
        throw omnetpp::cRuntimeError("The topic ID type flag is not correctly set to either topic name or short topic name");
}

std::string MqttSNUnsubscribe::getTopicName() const
{
    return topicName;
}

void MqttSNUnsubscribe::setTopicId(uint16_t id)
{
    if (id == UINT16_MAX)
        throw omnetpp::cRuntimeError("Reserved topic ID");

    if (getTopicIdTypeFlag() == TopicIdType::PRE_DEFINED_TOPIC_ID) {
        uint32_t field = topicId;
        MqttSNBase::setOptionalField(id, Length::TWO_OCTETS, field);
        topicId = field;
    }
    else {
        throw omnetpp::cRuntimeError("The topic ID type flag is not correctly set to predefined topic ID");
    }
}

uint16_t MqttSNUnsubscribe::getTopicId() const
{
    return topicId;
}

} /* namespace mqttsn */
