#ifndef MESSAGES_MQTTSNBASE_H_
#define MESSAGES_MQTTSNBASE_H_

#include "inet/common/packet/chunk/Chunk_m.h"
#include "types/MsgType.h"

namespace mqttsn {

class MqttSNBase : public inet::FieldsChunk
{
    private:
        std::vector<uint8_t> length;
        MsgType msgType;

        void setLength(uint16_t octets);

    protected:
        void addLength(uint16_t octets, uint16_t prevOctets = 0);
        std::string getClassName(std::string mangledName);

    public:
        MqttSNBase();

        uint16_t getLength();
        uint16_t getAvailableLength();

        void setMsgType(MsgType messageType);
        MsgType getMsgType() const;

        ~MqttSNBase() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASE_H_ */
