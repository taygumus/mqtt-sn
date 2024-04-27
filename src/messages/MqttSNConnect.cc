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

#include "MqttSNConnect.h"
#include "types/shared/Length.h"
#include "types/shared/Flag.h"

namespace mqttsn {

MqttSNConnect::MqttSNConnect()
{
    MqttSNBase::addLength(Length::TWO_OCTETS);
}

void MqttSNConnect::setWillFlag(bool willFlag)
{
    MqttSNBase::setBooleanFlag(willFlag, Flag::WILL, flags);
}

bool MqttSNConnect::getWillFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::WILL, flags);
}

void MqttSNConnect::setCleanSessionFlag(bool cleanSessionFlag)
{
    MqttSNBase::setBooleanFlag(cleanSessionFlag, Flag::CLEAN_SESSION, flags);
}

bool MqttSNConnect::getCleanSessionFlag() const
{
    return MqttSNBase::getBooleanFlag(Flag::CLEAN_SESSION, flags);
}

uint8_t MqttSNConnect::getProtocolId() const
{
    return protocolId;
}

void MqttSNConnect::setClientId(const std::string& id)
{
    MqttSNBase::setStringField(
            id,
            Length::ONE_OCTET,
            Length::CLIENT_ID_OCTETS,
            "Client ID length out of range",
            clientId
    );
}

std::string MqttSNConnect::getClientId() const
{
    return clientId;
}

} /* namespace mqttsn */
