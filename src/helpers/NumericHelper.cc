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

#include "NumericHelper.h"

namespace mqttsn {

void NumericHelper::incrementCounter(int* counter)
{
    if (*counter == std::numeric_limits<int>::max()) {
        *counter = 0;
    }
    else {
        (*counter)++;
    }
}

QoS NumericHelper::minQoS(QoS first, QoS second)
{
    // calculate the minimum QoS value
    int minQoSValue = std::min(
            ConversionHelper::qosToInt(first),
            ConversionHelper::qosToInt(second)
    );

    // convert the minimum QoS value back to QoS enumeration
    return ConversionHelper::intToQoS(minQoSValue);
}

} /* namespace mqttsn */
