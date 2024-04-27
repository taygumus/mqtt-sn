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

#ifndef MESSAGES_MQTTSNGWINFO_H_
#define MESSAGES_MQTTSNGWINFO_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNGwInfo : public MqttSNBase
{
    private:
        uint8_t gwId = 0;
        uint32_t gwAdd = 0;
        uint16_t gwPort = 0;

    public:
        MqttSNGwInfo();

        void setGwId(uint8_t gatewayId);
        uint8_t getGwId() const;

        void setGwAdd(const std::string& gatewayAddress);
        std::string getGwAdd() const;

        void setGwPort(uint16_t gatewayPort);
        uint16_t getGwPort() const;

        ~MqttSNGwInfo() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNGWINFO_H_ */
