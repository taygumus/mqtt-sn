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

#ifndef MESSAGES_MQTTSNCONNECT_H_
#define MESSAGES_MQTTSNCONNECT_H_

#include "MqttSNBaseWithDuration.h"

namespace mqttsn {

class MqttSNConnect : public MqttSNBaseWithDuration
{
    private:
        uint8_t flags = 0;
        uint8_t protocolId = 0x01;
        std::string clientId;

    public:
        MqttSNConnect();

        void setWillFlag(bool willFlag);
        bool getWillFlag() const;

        void setCleanSessionFlag(bool cleanSessionFlag);
        bool getCleanSessionFlag() const;

        uint8_t getProtocolId() const;

        void setClientId(const std::string& id);
        std::string getClientId() const;

        ~MqttSNConnect() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNCONNECT_H_ */
