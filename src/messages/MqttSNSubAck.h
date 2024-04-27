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

#ifndef MESSAGES_MQTTSNSUBACK_H_
#define MESSAGES_MQTTSNSUBACK_H_

#include "MqttSNMsgIdWithTopicIdPlus.h"
#include "types/shared/QoS.h"

namespace mqttsn {

class MqttSNSubAck : public MqttSNMsgIdWithTopicIdPlus
{
    private:
        uint8_t flags = 0;

    public:
        MqttSNSubAck();

        void setQoSFlag(QoS qosFlag);
        uint8_t getQoSFlag() const;

        ~MqttSNSubAck() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNSUBACK_H_ */
