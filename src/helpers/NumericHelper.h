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

#ifndef HELPERS_NUMERICHELPER_H_
#define HELPERS_NUMERICHELPER_H_

#include "BaseHelper.h"
#include "ConversionHelper.h"
#include "types/shared/QoS.h"

namespace mqttsn {

class NumericHelper : public BaseHelper
{
    public:
        static void incrementCounter(int* counter);
        static QoS minQoS(QoS first, QoS second);
};

} /* namespace mqttsn */

#endif /* HELPERS_NUMERICHELPER_H_ */
