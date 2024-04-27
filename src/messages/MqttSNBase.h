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

#ifndef MESSAGES_MQTTSNBASE_H_
#define MESSAGES_MQTTSNBASE_H_

#include "inet/common/packet/chunk/Chunk_m.h"
#include "types/shared/MsgType.h"
#include "types/shared/Flag.h"

namespace mqttsn {

class MqttSNBase : public inet::FieldsChunk
{
    private:
        std::vector<uint8_t> length;
        MsgType msgType;

    private:
        void setLength(uint16_t octets);

    protected:
        void addLength(uint16_t octets, uint16_t prevOctets = 0);

        void setOptionalField(uint32_t value, uint16_t octets, uint32_t& field);
        void setStringField(const std::string& value, uint16_t minLength, uint16_t maxLength, const std::string& error, std::string& field);

        void setFlag(uint8_t value, Flag position, uint8_t& flags);
        void setBooleanFlag(bool value, Flag position, uint8_t& flags);

        std::string getClassName(const std::string& mangledName) const;

        uint8_t getFlag(Flag position, uint8_t flags) const;
        bool getBooleanFlag(Flag position, uint8_t flags) const;

    public:
        MqttSNBase();

        uint16_t getLength() const;
        uint16_t getAvailableLength() const;

        void setMsgType(MsgType messageType);
        MsgType getMsgType() const;

        ~MqttSNBase() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASE_H_ */
