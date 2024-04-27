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

#include "MqttSNSearchGw.h"
#include "types/shared/Length.h"

namespace mqttsn {

MqttSNSearchGw::MqttSNSearchGw()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNSearchGw::setRadius(uint8_t hops)
{
    radius = hops;
}

uint8_t MqttSNSearchGw::getRadius() const
{
    return radius;
}

} /* namespace mqttsn */
