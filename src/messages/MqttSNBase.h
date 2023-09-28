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
        void setStringField(std::string value, uint16_t minLength, uint16_t maxLength, std::string error, std::string& field);

        void setFlag(uint8_t value, Flag position, uint8_t& flags);
        void setBooleanFlag(bool value, Flag position, uint8_t& flags);

        std::string getClassName(std::string mangledName) const;

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
