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

#ifndef MESSAGES_MQTTSNPINGREQ_H_
#define MESSAGES_MQTTSNPINGREQ_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNPingReq : public MqttSNBase
{
    private:
        std::string clientId;

    public:
        MqttSNPingReq() {};

        void setClientId(const std::string& id);
        std::string getClientId() const;

        ~MqttSNPingReq() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNPINGREQ_H_ */
