#ifndef MESSAGES_MQTTSNREGACK_H_
#define MESSAGES_MQTTSNREGACK_H_

#include "MqttSNMessage.h"
#include "types/ReturnCode.h"

namespace mqttsn {

class MqttSNRegAck : public MqttSNMessage
{
    private:
        uint16_t topicId = 0;
        uint16_t msgId = 0;
        ReturnCode returnCode;

    public:
        MqttSNRegAck();

        void setTopicId(uint16_t id);
        uint16_t getTopicId();

        void setMsgId(uint16_t messageId);
        uint16_t getMsgId();

        void setReturnCode(ReturnCode code);
        ReturnCode getReturnCode();

        ~MqttSNRegAck() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNREGACK_H_ */
