#ifndef MESSAGES_MQTTSNREGISTER_H_
#define MESSAGES_MQTTSNREGISTER_H_

#include "MqttSNMessage.h"

namespace mqttsn {

class MqttSNRegister : public MqttSNMessage
{
    private:
        uint16_t topicId = 0;
        uint16_t msgId = 0;
        std::string topicName;

    public:
        MqttSNRegister();

        void setTopicId(uint16_t id);
        uint16_t getTopicId();

        void setMsgId(uint16_t messageId);
        uint16_t getMsgId();

        void setTopicName(std::string name);
        std::string getTopicName();

        ~MqttSNRegister() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNREGISTER_H_ */
