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

#ifndef MESSAGES_MQTTSNSEARCHGW_H_
#define MESSAGES_MQTTSNSEARCHGW_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNSearchGw : public MqttSNBase
{
    private:
        uint8_t radius = 0;

    public:
        MqttSNSearchGw();

        void setRadius(uint8_t hops);
        uint8_t getRadius() const;

        ~MqttSNSearchGw() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNSEARCHGW_H_ */
