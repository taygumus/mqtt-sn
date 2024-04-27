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

#ifndef MESSAGES_MQTTSNADVERTISE_H_
#define MESSAGES_MQTTSNADVERTISE_H_

#include "MqttSNBaseWithDuration.h"

namespace mqttsn {

class MqttSNAdvertise : public MqttSNBaseWithDuration
{
    private:
        uint8_t gwId = 0;

    public:
        MqttSNAdvertise();

        void setGwId(uint8_t gatewayId);
        uint8_t getGwId() const;

        ~MqttSNAdvertise() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNADVERTISE_H_ */
