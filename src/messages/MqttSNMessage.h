#ifndef MESSAGES_MQTTSNMESSAGE_H_
#define MESSAGES_MQTTSNMESSAGE_H_

#include "inet/common/packet/chunk/Chunk_m.h"
#include "types/MsgType.h"

namespace mqttsn {

class MqttSNMessage : public inet::FieldsChunk
{
    private:
        std::vector<uint8_t> length;
        MsgType msgType;

    public:
        MqttSNMessage() {};

        void setLength(unsigned __int16 length);
        unsigned __int16 getLength();

        void setMsgType(MsgType msg);
        MsgType getMsgType();

        ~MqttSNMessage() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNMESSAGE_H_ */
