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

#ifndef MESSAGES_MQTTSNMSGIDWITHTOPICIDPLUS_H_
#define MESSAGES_MQTTSNMSGIDWITHTOPICIDPLUS_H_

#include "MqttSNMsgIdWithTopicId.h"
#include "types/shared/ReturnCode.h"

namespace mqttsn {

class MqttSNMsgIdWithTopicIdPlus : public MqttSNMsgIdWithTopicId
{
    private:
        ReturnCode returnCode;

    public:
        MqttSNMsgIdWithTopicIdPlus();

        void setReturnCode(ReturnCode code);
        ReturnCode getReturnCode() const;

        ~MqttSNMsgIdWithTopicIdPlus() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNMSGIDWITHTOPICIDPLUS_H_ */
