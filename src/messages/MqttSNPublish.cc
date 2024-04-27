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

#include "MqttSNPublish.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNPublish::MqttSNPublish()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNPublish::setDupFlag(bool dupFlag)
{
    MqttSNBase::setBooleanFlag(dupFlag, Flag::DUP, flags);
}

bool MqttSNPublish::getDupFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::DUP, flags);
}

void MqttSNPublish::setQoSFlag(QoS qosFlag)
{
    MqttSNBase::setFlag(qosFlag, Flag::QUALITY_OF_SERVICE, flags);
}

uint8_t MqttSNPublish::getQoSFlag() const
{
    return MqttSNBase::getFlag(Flag::QUALITY_OF_SERVICE, flags);
}

void MqttSNPublish::setRetainFlag(bool retainFlag)
{
    MqttSNBase::setBooleanFlag(retainFlag, Flag::RETAIN, flags);
}

bool MqttSNPublish::getRetainFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::RETAIN, flags);
}

void MqttSNPublish::setTopicIdTypeFlag(TopicIdType topicIdTypeFlag)
{
    MqttSNBase::setFlag(topicIdTypeFlag, Flag::TOPIC_ID_TYPE, flags);
}

uint8_t MqttSNPublish::getTopicIdTypeFlag() const
{
    return MqttSNBase::getFlag(Flag::TOPIC_ID_TYPE, flags);
}

void MqttSNPublish::setData(const std::string& stringData)
{
    MqttSNBase::setStringField(
            stringData,
            Length::ZERO_OCTETS,
            MqttSNBase::getAvailableLength(),
            "Data string length out of range",
            data
    );
}

std::string MqttSNPublish::getData() const
{
    return data;
}

} /* namespace mqttsn */
