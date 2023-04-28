#ifndef MESSAGES_MQTTSNMESSAGE_H_
#define MESSAGES_MQTTSNMESSAGE_H_

#include "inet/common/packet/chunk/Chunk_m.h"
#include "types/MsgType.h"

namespace mqttsn {

class MqttSNMessage : public inet::FieldsChunk
{
    private:
        std::vector<uint8_t> length = {0x02};
        MsgType msgType;

    protected:
        void setLength(uint16_t fixedLength, uint16_t variableLength);

    public:
        MqttSNMessage() {};

        uint16_t getLength();

        void setMsgType(MsgType msg);
        MsgType getMsgType();

        ~MqttSNMessage() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNMESSAGE_H_ */
