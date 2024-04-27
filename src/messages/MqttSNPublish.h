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

#ifndef MESSAGES_MQTTSNPUBLISH_H_
#define MESSAGES_MQTTSNPUBLISH_H_

#include "MqttSNMsgIdWithTopicId.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"

namespace mqttsn {

class MqttSNPublish : public MqttSNMsgIdWithTopicId
{
    private:
        uint8_t flags = 0;
        std::string data;

    public:
        MqttSNPublish();

        void setDupFlag(bool dupFlag);
        bool getDupFlag() const;

        void setQoSFlag(QoS qosFlag);
        uint8_t getQoSFlag() const;

        void setRetainFlag(bool retainFlag);
        bool getRetainFlag() const;

        void setTopicIdTypeFlag(TopicIdType topicIdTypeFlag);
        uint8_t getTopicIdTypeFlag() const;

        void setData(const std::string& stringData);
        std::string getData() const;

        ~MqttSNPublish() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNPUBLISH_H_ */
