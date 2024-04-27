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

#ifndef MESSAGES_MQTTSNBASEWITHMSGID_H_
#define MESSAGES_MQTTSNBASEWITHMSGID_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNBaseWithMsgId : public MqttSNBase
{
    private:
        uint16_t msgId = 0;

    public:
        MqttSNBaseWithMsgId();

        void setMsgId(uint16_t messageId);
        uint16_t getMsgId() const;

        ~MqttSNBaseWithMsgId() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASEWITHMSGID_H_ */
