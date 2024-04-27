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

#ifndef MESSAGES_MQTTSNMSGIDWITHTOPICID_H_
#define MESSAGES_MQTTSNMSGIDWITHTOPICID_H_

#include "MqttSNBaseWithMsgId.h"

namespace mqttsn {

class MqttSNMsgIdWithTopicId : public MqttSNBaseWithMsgId
{
    private:
        uint16_t topicId = 0;

    public:
        MqttSNMsgIdWithTopicId();

        void setTopicId(uint16_t id);
        uint16_t getTopicId() const;

        ~MqttSNMsgIdWithTopicId() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNMSGIDWITHTOPICID_H_ */
